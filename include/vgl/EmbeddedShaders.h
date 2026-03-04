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
out float v_fragW;

void main() {
  vec4 worldPos = model * vec4(aPos, 1.0);
  FragPos = worldPos.xyz;
  Normal = mat3(transpose(inverse(model))) * aNormal;
  gl_Position = projection * view * worldPos;
  v_fragW = gl_Position.w;
}
)";

inline const char* defaultFrag = R"(
#version 330 core

in vec3 FragPos;
in vec3 Normal;
in float v_fragW;

uniform vec3 color;
uniform vec3 lightDir;
uniform vec3 viewPos;
uniform bool useLighting;
uniform float logDepthFarPlane;

out vec4 FragColor;

void main() {
  // Logarithmic depth buffer — eliminates z-fighting across huge depth ranges.
  // Set logDepthFarPlane > 0 to enable; 0 = standard hardware depth.
  if (logDepthFarPlane > 0.0) {
    gl_FragDepth = log2(max(1e-6, 1.0 + v_fragW)) / log2(1.0 + logDepthFarPlane);
  } else {
    gl_FragDepth = gl_FragCoord.z;
  }

  if (!useLighting) {
    FragColor = vec4(color, 1.0);
    return;
  }

  vec3 norm = normalize(Normal);
  vec3 light = normalize(lightDir);

  float ambient = 0.15;
  float diffuse = max(dot(norm, light), 0.0);

  vec3 viewDir = normalize(viewPos - FragPos);
  vec3 halfDir = normalize(light + viewDir);
  float specular = pow(max(dot(norm, halfDir), 0.0), 32.0) * 0.3;

  vec3 result = color * (ambient + diffuse) + vec3(specular);
  FragColor = vec4(result, 1.0);
}
)";

} // namespace EmbeddedShaders

#endif
