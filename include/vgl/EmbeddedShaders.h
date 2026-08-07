#ifndef EMBEDDED_SHADERS_H
#define EMBEDDED_SHADERS_H

namespace EmbeddedShaders {

inline const char* defaultVert = R"(
#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;
out float v_fragW;

void main() {
  vec4 worldPos = model * vec4(aPos, 1.0);
  FragPos  = worldPos.xyz;
  Normal   = mat3(transpose(inverse(model))) * aNormal;
  TexCoord = aTexCoord;
  gl_Position = projection * view * worldPos;
  v_fragW = gl_Position.w;
}
)";

inline const char* defaultFrag = R"(
#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;
in float v_fragW;

uniform vec3      color;
uniform vec3      lightDir;
uniform vec3      viewPos;
uniform bool      useLighting;
uniform bool      useTexture;
uniform sampler2D uTexture;
uniform float     logDepthFarPlane;
uniform float     ambientLight;

out vec4 FragColor;

void main() {
  // Logarithmic depth buffer — eliminates z-fighting across huge depth ranges.
  // Set logDepthFarPlane > 0 to enable; 0 = standard hardware depth.
  if (logDepthFarPlane > 0.0) {
    gl_FragDepth = log2(max(1e-6, 1.0 + v_fragW)) / log2(1.0 + logDepthFarPlane);
  } else {
    gl_FragDepth = gl_FragCoord.z;
  }

  vec3 baseColor = useTexture ? texture(uTexture, TexCoord).rgb : color;

  if (!useLighting) {
    FragColor = vec4(baseColor, 1.0);
    return;
  }

  vec3 norm = normalize(Normal);
  vec3 light = normalize(lightDir);

  float diffuse = max(dot(norm, light), 0.0);

  vec3 viewDir = normalize(viewPos - FragPos);
  vec3 halfDir = normalize(light + viewDir);
  float specular = pow(max(dot(norm, halfDir), 0.0), 32.0) * 0.3;

  vec3 result = baseColor * (ambientLight + diffuse) + vec3(specular);
  FragColor = vec4(result, 1.0);
}
)";

// A true infinite ground plane: rendered as a full-screen pass (reusing
// GUI's unit quad mesh, but the vertex shader ignores model/view/projection
// entirely and just maps it straight to NDC), then the fragment shader
// casts a per-pixel ray from the camera and analytically intersects it with
// the world-space plane z = planeZ. Pixels whose ray doesn't hit the plane
// (in front of the camera, within maxDistance) are discarded, showing
// whatever was drawn/cleared behind. Because the plane is a math equation,
// not a giant mesh with vertices far from the origin, there's no
// floating-point/depth-buffer precision breakdown at distance the way a
// literal huge box has.
inline const char* groundPlaneVert = R"(
#version 330 core

layout(location = 0) in vec3 aPos; // GUI's unit quad, -0.5..0.5

out vec2 vNdc;

void main() {
  vNdc = aPos.xy * 2.0; // map straight to NDC (-1..1), covering the full screen
  gl_Position = vec4(vNdc, 0.0, 1.0);
}
)";

inline const char* groundPlaneFrag = R"(
#version 330 core

in vec2 vNdc;
out vec4 FragColor;

// Ray direction is reconstructed from the camera's own basis vectors and
// FOV, NOT by inverting the view-projection matrix. At the near/far ratios
// these scenes use (near=1, far=2e7), glm::inverse() on the projection
// matrix loses catastrophic precision in float32 -- that was the original
// bug here (the whole screen came out green: the bad inverse made nearly
// every pixel's ray, including ones that should point at the sky, appear
// to hit the plane in front of the camera). Camera position/basis/FOV have
// no such extreme dynamic range, so this reconstruction is well-conditioned
// regardless of far-plane distance.
uniform vec3  cameraPos;
uniform vec3  camForward;
uniform vec3  camRight;
uniform vec3  camUp;
uniform float tanHalfFovY;
uniform float aspect;

uniform mat4  viewProj; // forward projection only, for depth -- safe, no inverse needed
uniform vec3  groundColor;
uniform vec3  fadeColor;
uniform float planeZ;
uniform float maxDistance;
uniform float logDepthFarPlane;

void main() {
  float px = vNdc.x * tanHalfFovY * aspect;
  float py = vNdc.y * tanHalfFovY;
  vec3 rayDir = normalize(camForward + px * camRight + py * camUp);
  vec3 rayOrigin = cameraPos;

  if (abs(rayDir.z) < 1e-6) discard; // ray parallel to the plane
  float t = (planeZ - rayOrigin.z) / rayDir.z;
  if (t < 0.0) discard; // plane is behind the camera

  vec3 worldPos = rayOrigin + rayDir * t;
  float dist = distance(worldPos, cameraPos);
  if (dist > maxDistance) discard;

  // Recompute this pixel's real depth from the intersection point so the
  // plane correctly occludes/is occluded by ordinary scene geometry.
  vec4 clip = viewProj * vec4(worldPos, 1.0);
  if (logDepthFarPlane > 0.0) {
    gl_FragDepth = log2(max(1e-6, 1.0 + clip.w)) / log2(1.0 + logDepthFarPlane);
  } else {
    gl_FragDepth = (clip.z / clip.w) * 0.5 + 0.5;
  }

  float fade = smoothstep(maxDistance * 0.75, maxDistance, dist);
  FragColor = vec4(mix(groundColor, fadeColor, fade), 1.0);
}
)";

} // namespace EmbeddedShaders

#endif
