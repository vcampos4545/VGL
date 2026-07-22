#ifndef VGL_POINT_BATCH_2D_H
#define VGL_POINT_BATCH_2D_H

#include <vgl/Shader.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

// Instanced 2D circle/point renderer for particle systems and other
// large point sets. A single small circle mesh is instanced with a
// per-instance position, radius, and color via glVertexAttribDivisor,
// so thousands of points cost one draw call.
class PointBatch2D {
public:
  explicit PointBatch2D(int circleSegments = 10);
  ~PointBatch2D();

  PointBatch2D(const PointBatch2D &) = delete;
  PointBatch2D &operator=(const PointBatch2D &) = delete;

  void begin();
  void addPoint(glm::vec2 pos, float radius, glm::vec3 color);
  void end();

  void draw(const glm::mat4 &viewProj) const;

private:
  struct Instance {
    glm::vec2 pos;
    float radius;
    glm::vec3 color;
  };

  std::vector<Instance> m_instances;
  Shader m_shader;

  GLuint m_vao = 0;
  GLuint m_circleVbo = 0;
  GLuint m_ebo = 0;
  GLuint m_instanceVbo = 0;
  unsigned int m_circleIndexCount = 0;

  size_t m_capacity = 0;
  size_t m_drawCount = 0;
};

#endif
