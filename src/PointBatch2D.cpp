#include <vgl/PointBatch2D.h>
#include <vgl/EmbeddedShaders2D.h>
#include <cmath>

namespace {
constexpr float PI = 3.14159265359f;
}

PointBatch2D::PointBatch2D(int circleSegments) {
  m_shader.loadFromSource(EmbeddedShaders2D::pointVert, EmbeddedShaders2D::pointFrag);

  // Unit circle base mesh (radius 1, triangle fan via indices), shared by all instances.
  std::vector<glm::vec2> circleVerts;
  std::vector<unsigned int> circleIndices;
  circleVerts.push_back({0.0f, 0.0f});
  for (int i = 0; i <= circleSegments; ++i) {
    float angle = 2.0f * PI * i / circleSegments;
    circleVerts.push_back({std::cos(angle), std::sin(angle)});
  }
  for (int i = 1; i <= circleSegments; ++i) {
    circleIndices.push_back(0);
    circleIndices.push_back(i);
    circleIndices.push_back(i + 1);
  }
  m_circleIndexCount = static_cast<unsigned int>(circleIndices.size());

  glGenVertexArrays(1, &m_vao);
  glGenBuffers(1, &m_circleVbo);
  glGenBuffers(1, &m_ebo);
  glGenBuffers(1, &m_instanceVbo);

  glBindVertexArray(m_vao);

  glBindBuffer(GL_ARRAY_BUFFER, m_circleVbo);
  glBufferData(GL_ARRAY_BUFFER, circleVerts.size() * sizeof(glm::vec2), circleVerts.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void *)0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, circleIndices.size() * sizeof(unsigned int), circleIndices.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, m_instanceVbo);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Instance), (void *)offsetof(Instance, pos));
  glEnableVertexAttribArray(1);
  glVertexAttribDivisor(1, 1);
  glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Instance), (void *)offsetof(Instance, radius));
  glEnableVertexAttribArray(2);
  glVertexAttribDivisor(2, 1);
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Instance), (void *)offsetof(Instance, color));
  glEnableVertexAttribArray(3);
  glVertexAttribDivisor(3, 1);

  glBindVertexArray(0);
}

PointBatch2D::~PointBatch2D() {
  if (m_instanceVbo) glDeleteBuffers(1, &m_instanceVbo);
  if (m_ebo) glDeleteBuffers(1, &m_ebo);
  if (m_circleVbo) glDeleteBuffers(1, &m_circleVbo);
  if (m_vao) glDeleteVertexArrays(1, &m_vao);
}

void PointBatch2D::begin() {
  m_instances.clear();
}

void PointBatch2D::addPoint(glm::vec2 pos, float radius, glm::vec3 color) {
  m_instances.push_back({pos, radius, color});
}

void PointBatch2D::end() {
  glBindBuffer(GL_ARRAY_BUFFER, m_instanceVbo);
  m_drawCount = m_instances.size();

  if (m_drawCount > m_capacity) {
    m_capacity = m_drawCount + m_drawCount / 2 + 256;
    glBufferData(GL_ARRAY_BUFFER, m_capacity * sizeof(Instance), nullptr, GL_DYNAMIC_DRAW);
  }
  if (m_drawCount > 0) {
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_drawCount * sizeof(Instance), m_instances.data());
  }
}

void PointBatch2D::draw(const glm::mat4 &viewProj) const {
  if (m_drawCount == 0) return;

  m_shader.use();
  m_shader.setMat4("viewProj", viewProj);

  glBindVertexArray(m_vao);
  glDrawElementsInstanced(GL_TRIANGLES, m_circleIndexCount, GL_UNSIGNED_INT, 0,
                           static_cast<GLsizei>(m_drawCount));
  glBindVertexArray(0);
}
