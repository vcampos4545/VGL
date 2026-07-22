#include <vgl/LineBatch2D.h>
#include <vgl/EmbeddedShaders2D.h>

LineBatch2D::LineBatch2D() {
  m_shader.loadFromSource(EmbeddedShaders2D::lineVert, EmbeddedShaders2D::lineFrag);

  glGenVertexArrays(1, &m_vao);
  glGenBuffers(1, &m_vbo);

  glBindVertexArray(m_vao);
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(LineVertex), (void *)offsetof(LineVertex, pos));
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(LineVertex), (void *)offsetof(LineVertex, color));
  glEnableVertexAttribArray(1);
  glBindVertexArray(0);
}

LineBatch2D::~LineBatch2D() {
  if (m_vbo) glDeleteBuffers(1, &m_vbo);
  if (m_vao) glDeleteVertexArrays(1, &m_vao);
}

void LineBatch2D::begin() {
  m_vertices.clear();
}

void LineBatch2D::addLine(glm::vec2 a, glm::vec2 b, glm::vec3 color) {
  m_vertices.push_back({a, color});
  m_vertices.push_back({b, color});
}

void LineBatch2D::addPolyline(const std::vector<glm::vec2> &points, glm::vec3 color, bool closed) {
  if (points.size() < 2) return;
  for (size_t i = 0; i + 1 < points.size(); ++i) {
    addLine(points[i], points[i + 1], color);
  }
  if (closed) {
    addLine(points.back(), points.front(), color);
  }
}

void LineBatch2D::end() {
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  m_drawCount = m_vertices.size();

  if (m_drawCount > m_capacity) {
    // Grow with headroom so we don't reallocate every frame as the scene changes.
    m_capacity = m_drawCount + m_drawCount / 2 + 64;
    glBufferData(GL_ARRAY_BUFFER, m_capacity * sizeof(LineVertex), nullptr, GL_DYNAMIC_DRAW);
  }
  if (m_drawCount > 0) {
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_drawCount * sizeof(LineVertex), m_vertices.data());
  }
}

void LineBatch2D::draw(const glm::mat4 &viewProj, float lineWidth) const {
  if (m_drawCount == 0) return;

  m_shader.use();
  m_shader.setMat4("viewProj", viewProj);
  glLineWidth(lineWidth);

  glBindVertexArray(m_vao);
  glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(m_drawCount));
  glBindVertexArray(0);
}
