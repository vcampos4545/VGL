#ifndef MESH_H
#define MESH_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 uv;
};

class Mesh {
public:
  Mesh();
  ~Mesh();
  Mesh(Mesh&& other) noexcept;
  Mesh& operator=(Mesh&& other) noexcept;
  Mesh(const Mesh&) = delete;
  Mesh& operator=(const Mesh&) = delete;

  void upload(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
  void uploadLines(const std::vector<glm::vec3>& points);
  void draw() const;
  void drawLines() const;
  bool isUploaded() const { return m_vao != 0; }

private:
  void cleanup();
  void setupAttributes();

  GLuint m_vao = 0;
  GLuint m_vbo = 0;
  GLuint m_ebo = 0;
  unsigned int m_indexCount = 0;
  unsigned int m_vertexCount = 0;
  bool m_isLineMode = false;
};

namespace MeshGen {
  void circle(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, int segments = 32);
  void quad(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);
  void cube(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);
  void sphere(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, int rings = 16, int sectors = 32);
  void cylinder(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, int segments = 32);

  // A flat annulus (ring) in the local XY plane, normal +Z, outer radius 1
  // and inner radius innerRadiusRatio * 1 (0..1). UVs run angularly (u, 0..1
  // around) and radially (v: 0 at the inner edge, 1 at the outer edge) --
  // matching the common "radial strip" convention for planetary ring
  // textures, so a simple 1D-ish gradient texture wraps correctly.
  void ring(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, float innerRadiusRatio = 0.5f, int segments = 128);
}

#endif
