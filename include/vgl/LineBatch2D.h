#ifndef VGL_LINE_BATCH_2D_H
#define VGL_LINE_BATCH_2D_H

#include <vgl/Shader.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

// Batched 2D line-segment renderer. Accumulate segments with addLine /
// addPolyline each frame, then upload once and issue a single GL_LINES
// draw call. Line width is applied uniformly per draw() call (OpenGL core
// profile only guarantees width=1 on most drivers, but this is set via
// glLineWidth for drivers that honor it).
class LineBatch2D {
public:
  LineBatch2D();
  ~LineBatch2D();

  LineBatch2D(const LineBatch2D &) = delete;
  LineBatch2D &operator=(const LineBatch2D &) = delete;

  void begin();
  void addLine(glm::vec2 a, glm::vec2 b, glm::vec3 color);
  void addPolyline(const std::vector<glm::vec2> &points, glm::vec3 color, bool closed = false);
  void end();

  void draw(const glm::mat4 &viewProj, float lineWidth = 1.0f) const;

private:
  struct LineVertex {
    glm::vec2 pos;
    glm::vec3 color;
  };

  std::vector<LineVertex> m_vertices;
  Shader m_shader;
  GLuint m_vao = 0;
  GLuint m_vbo = 0;
  size_t m_capacity = 0;
  size_t m_drawCount = 0;
};

#endif
