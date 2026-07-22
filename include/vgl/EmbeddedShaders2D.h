#ifndef VGL_EMBEDDED_SHADERS_2D_H
#define VGL_EMBEDDED_SHADERS_2D_H

// Embedded GLSL sources for VGL's 2D rendering module (scalar-field
// colormap quads, batched lines, instanced points, flat-filled polygons).
// Kept separate from EmbeddedShaders.h (the 3D/lit default shader) so the
// two rendering paths can evolve independently.
namespace EmbeddedShaders2D {

// --- Scalar field (colormap) shader ---
// Renders a world-space rectangle textured with a single-channel float
// field, mapped through a selectable colormap between [uMin, uMax].
// Optionally composites a solid/obstacle mask on top and draws contour
// isolines.

inline const char *fieldVert = R"(
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aUV;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 vUV;

void main() {
  vUV = aUV;
  gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)";

inline const char *fieldFrag = R"(
#version 330 core
in vec2 vUV;
out vec4 FragColor;

uniform sampler2D uField;
uniform sampler2D uMask;
uniform float uMin;
uniform float uMax;
uniform int  uColormap; // 0=viridis 1=jet 2=coolwarm 3=grayscale 4=fire
uniform bool uUseMask;
uniform bool uContours;
uniform int  uContourCount;
uniform vec3 uSolidColor;

vec3 viridis(float t) {
  vec3 c0 = vec3(0.267, 0.005, 0.329);
  vec3 c1 = vec3(0.229, 0.322, 0.545);
  vec3 c2 = vec3(0.128, 0.567, 0.551);
  vec3 c3 = vec3(0.369, 0.789, 0.383);
  vec3 c4 = vec3(0.993, 0.906, 0.144);
  float x = clamp(t, 0.0, 1.0) * 4.0;
  if (x < 1.0) return mix(c0, c1, x);
  if (x < 2.0) return mix(c1, c2, x - 1.0);
  if (x < 3.0) return mix(c2, c3, x - 2.0);
  return mix(c3, c4, x - 3.0);
}

vec3 jetColor(float t) {
  t = clamp(t, 0.0, 1.0);
  float r = clamp(1.5 - abs(4.0 * t - 3.0), 0.0, 1.0);
  float g = clamp(1.5 - abs(4.0 * t - 2.0), 0.0, 1.0);
  float b = clamp(1.5 - abs(4.0 * t - 1.0), 0.0, 1.0);
  return vec3(r, g, b);
}

vec3 coolwarm(float t) {
  vec3 cold = vec3(0.230, 0.299, 0.754);
  vec3 mid  = vec3(0.865, 0.865, 0.865);
  vec3 warm = vec3(0.706, 0.016, 0.150);
  t = clamp(t, 0.0, 1.0);
  return t < 0.5 ? mix(cold, mid, t * 2.0) : mix(mid, warm, (t - 0.5) * 2.0);
}

vec3 fireColor(float t) {
  vec3 c0 = vec3(0.0, 0.0, 0.0);
  vec3 c1 = vec3(0.5, 0.0, 0.0);
  vec3 c2 = vec3(1.0, 0.4, 0.0);
  vec3 c3 = vec3(1.0, 1.0, 0.0);
  vec3 c4 = vec3(1.0, 1.0, 1.0);
  float x = clamp(t, 0.0, 1.0) * 4.0;
  if (x < 1.0) return mix(c0, c1, x);
  if (x < 2.0) return mix(c1, c2, x - 1.0);
  if (x < 3.0) return mix(c2, c3, x - 2.0);
  return mix(c3, c4, x - 3.0);
}

vec3 colormap(float t, int id) {
  if (id == 0) return viridis(t);
  if (id == 1) return jetColor(t);
  if (id == 2) return coolwarm(t);
  if (id == 3) return vec3(clamp(t, 0.0, 1.0));
  return fireColor(t);
}

void main() {
  float v = texture(uField, vUV).r;
  float t = (v - uMin) / max(uMax - uMin, 1e-6);
  vec3 col = colormap(t, uColormap);

  if (uContours) {
    float scaled = clamp(t, 0.0, 1.0) * float(uContourCount);
    float dist = abs(fract(scaled) - 0.5) * 2.0;
    float edge = smoothstep(0.0, 0.12, dist);
    col *= mix(0.35, 1.0, edge);
  }

  if (uUseMask) {
    float solid = texture(uMask, vUV).r;
    col = mix(col, uSolidColor, clamp(solid, 0.0, 1.0));
  }

  FragColor = vec4(col, 1.0);
}
)";

// --- Batched line shader (per-vertex color, world space) ---

inline const char *lineVert = R"(
#version 330 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec3 aColor;

uniform mat4 viewProj;

out vec3 vColor;

void main() {
  vColor = aColor;
  gl_Position = viewProj * vec4(aPos, 0.0, 1.0);
}
)";

inline const char *lineFrag = R"(
#version 330 core
in vec3 vColor;
out vec4 FragColor;
void main() { FragColor = vec4(vColor, 1.0); }
)";

// --- Instanced point/circle shader (particles) ---

inline const char *pointVert = R"(
#version 330 core
layout(location = 0) in vec2 aBasePos;    // unit circle, per-vertex
layout(location = 1) in vec2 aInstPos;    // per-instance world position
layout(location = 2) in float aInstRadius;// per-instance radius
layout(location = 3) in vec3 aInstColor;  // per-instance color

uniform mat4 viewProj;

out vec3 vColor;

void main() {
  vColor = aInstColor;
  vec2 worldPos = aInstPos + aBasePos * aInstRadius;
  gl_Position = viewProj * vec4(worldPos, 0.0, 1.0);
}
)";

inline const char *pointFrag = R"(
#version 330 core
in vec3 vColor;
out vec4 FragColor;
void main() { FragColor = vec4(vColor, 1.0); }
)";

// --- Flat-filled polygon shader (single solid color, world space) ---

inline const char *flatVert = R"(
#version 330 core
layout(location = 0) in vec2 aPos;

uniform mat4 viewProj;

void main() {
  gl_Position = viewProj * vec4(aPos, 0.0, 1.0);
}
)";

inline const char *flatFrag = R"(
#version 330 core
out vec4 FragColor;
uniform vec3 uColor;
uniform float uAlpha;
void main() { FragColor = vec4(uColor, uAlpha); }
)";

} // namespace EmbeddedShaders2D

#endif
