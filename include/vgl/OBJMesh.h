#ifndef OBJMESH_H
#define OBJMESH_H

#include <vgl/Mesh.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <unordered_map>

struct Material
{
  std::string name;
  glm::vec3 diffuse{0.8f, 0.8f, 0.8f};
  glm::vec3 ambient{0.2f, 0.2f, 0.2f};
  glm::vec3 specular{1.0f, 1.0f, 1.0f};
  float shininess = 32.0f;
};

struct SubMesh
{
  Mesh mesh;
  Material material;
};

class OBJMesh
{
public:
  OBJMesh() = default;
  ~OBJMesh() = default;

  OBJMesh(OBJMesh &&) = default;
  OBJMesh &operator=(OBJMesh &&) = default;
  OBJMesh(const OBJMesh &) = delete;
  OBJMesh &operator=(const OBJMesh &) = delete;

  bool load(const std::string &path);
  bool isLoaded() const { return !m_subMeshes.empty(); }

  const std::vector<SubMesh> &getSubMeshes() const { return m_subMeshes; }
  const std::string &getError() const { return m_error; }

  // Axis-aligned bounding box of the raw OBJ vertex positions, in the
  // model's own local space (before any draw-time scale/rotation/translate).
  // Lets a caller fit a loaded model to a known real-world size without
  // having to already know how the source file was authored/scaled.
  glm::vec3 getBoundsMin() const { return m_boundsMin; }
  glm::vec3 getBoundsMax() const { return m_boundsMax; }

private:
  bool loadMTL(const std::string &path);
  void buildMeshes(
      const std::vector<glm::vec3> &positions,
      const std::vector<glm::vec3> &normals,
      const std::vector<glm::vec2> &texCoords,
      const std::vector<std::tuple<std::string, std::vector<std::array<int, 9>>>> &materialFaces);

  std::vector<SubMesh> m_subMeshes;
  std::unordered_map<std::string, Material> m_materials;
  std::string m_error;
  glm::vec3 m_boundsMin{0.0f};
  glm::vec3 m_boundsMax{0.0f};
};

#endif
