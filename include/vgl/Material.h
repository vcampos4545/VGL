#ifndef VGL_MATERIAL_H
#define VGL_MATERIAL_H

#include <glm/glm.hpp>
#include <string>

// Per-object surface lighting properties, passed to GUI's 3D draw calls
// (default-constructed if the caller omits it) and populated per-submesh
// from .mtl data by OBJMesh. ambient/specular follow the OBJ/MTL convention
// of being *multipliers* on the scene's ambient/specular light, not
// standalone colors, so an unset Ka/Ks in a .mtl file (which defaults here
// to a neutral tint) doesn't unexpectedly darken a mesh.
struct Material {
  std::string name;
  glm::vec3 diffuse{0.8f, 0.8f, 0.8f};    // base color, used when a draw call doesn't pass its own color (OBJMesh path)
  glm::vec3 ambient{1.0f, 1.0f, 1.0f};    // tints the scene's ambient (hemisphere) light
  glm::vec3 specular{0.3f, 0.3f, 0.3f};   // tints/strength of the specular highlight
  float shininess = 32.0f;                // specular highlight size (higher = smaller/sharper)
  bool lit = true;                        // false = flat unlit color, ignoring scene lighting entirely

  static Material Unlit() {
    Material m;
    m.lit = false;
    return m;
  }
};

#endif
