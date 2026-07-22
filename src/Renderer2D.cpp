#include <vgl/Renderer2D.h>
#include <vgl/EmbeddedShaders2D.h>
#include <vgl/Triangulation.h>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

namespace {
constexpr float PI = 3.14159265359f;
}

Renderer2D::Renderer2D(GUI &gui) : m_gui(gui) {
  m_fieldShader.loadFromSource(EmbeddedShaders2D::fieldVert, EmbeddedShaders2D::fieldFrag);
  m_flatShader.loadFromSource(EmbeddedShaders2D::flatVert, EmbeddedShaders2D::flatFrag);

  std::vector<Vertex> verts;
  std::vector<unsigned int> idx;
  MeshGen::quad(verts, idx);
  m_quadMesh.upload(verts, idx);

  glGenVertexArrays(1, &m_flatVao);
  glGenBuffers(1, &m_flatVbo);
  glGenBuffers(1, &m_flatEbo);
  glBindVertexArray(m_flatVao);
  glBindBuffer(GL_ARRAY_BUFFER, m_flatVbo);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void *)0);
  glEnableVertexAttribArray(0);
  glBindVertexArray(0);
}

void Renderer2D::beginFrame(glm::vec3 clearColor) {
  m_gui.beginFrame();
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glClearColor(clearColor.r, clearColor.g, clearColor.b, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  m_lineBatch.begin();
  m_pointBatch.begin();
}

void Renderer2D::flush() {
  glm::mat4 vp = camera.getViewProjection(getAspect());

  m_lineBatch.end();
  m_lineBatch.draw(vp, m_lineWidth);

  m_pointBatch.end();
  m_pointBatch.draw(vp);
}

void Renderer2D::present() {
  glEnable(GL_DEPTH_TEST);
  m_gui.endFrame();
}

void Renderer2D::endFrame() {
  flush();
  present();
}

void Renderer2D::drawScalarField(const DataTexture &field, glm::vec2 origin, glm::vec2 size,
                                  float vmin, float vmax, Colormap map,
                                  const DataTexture *solidMask,
                                  bool contours, int contourCount,
                                  glm::vec3 solidColor) {
  glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(origin + size * 0.5f, 0.0f));
  model = glm::scale(model, glm::vec3(size, 1.0f));

  m_fieldShader.use();
  m_fieldShader.setMat4("model", model);
  m_fieldShader.setMat4("view", camera.getViewMatrix());
  m_fieldShader.setMat4("projection", camera.getProjectionMatrix(getAspect()));
  m_fieldShader.setFloat("uMin", vmin);
  m_fieldShader.setFloat("uMax", vmax);
  m_fieldShader.setInt("uColormap", static_cast<int>(map));
  m_fieldShader.setBool("uContours", contours);
  m_fieldShader.setInt("uContourCount", contourCount);
  m_fieldShader.setVec3("uSolidColor", solidColor);
  m_fieldShader.setBool("uUseMask", solidMask != nullptr);

  field.bind(0);
  m_fieldShader.setInt("uField", 0);
  if (solidMask) {
    solidMask->bind(1);
    m_fieldShader.setInt("uMask", 1);
  }

  m_quadMesh.draw();
}

void Renderer2D::drawLine(glm::vec2 a, glm::vec2 b, glm::vec3 color, float width) {
  m_lineWidth = width;
  m_lineBatch.addLine(a, b, color);
}

void Renderer2D::drawPolyline(const std::vector<glm::vec2> &pts, glm::vec3 color, float width, bool closed) {
  m_lineWidth = width;
  m_lineBatch.addPolyline(pts, color, closed);
}

void Renderer2D::drawArrow(glm::vec2 from, glm::vec2 to, glm::vec3 color, float width, float headSize) {
  glm::vec2 dir = to - from;
  float len = glm::length(dir);
  if (len < 1e-6f) return;
  glm::vec2 dirN = dir / len;
  glm::vec2 perp(-dirN.y, dirN.x);

  float head = glm::min(headSize, len * 0.5f);
  glm::vec2 shaftEnd = to - dirN * head;

  drawLine(from, shaftEnd, color, width);
  drawLine(to, shaftEnd + perp * head * 0.5f, color, width);
  drawLine(to, shaftEnd - perp * head * 0.5f, color, width);
}

void Renderer2D::drawCircleOutline(glm::vec2 pos, float radius, glm::vec3 color, float width, int segments) {
  std::vector<glm::vec2> pts;
  pts.reserve(segments);
  for (int i = 0; i < segments; ++i) {
    float angle = 2.0f * PI * i / segments;
    pts.push_back(pos + radius * glm::vec2(std::cos(angle), std::sin(angle)));
  }
  drawPolyline(pts, color, width, true);
}

void Renderer2D::drawRectOutline(glm::vec2 center, glm::vec2 size, glm::vec3 color, float width) {
  glm::vec2 h = size * 0.5f;
  std::vector<glm::vec2> pts = {
      center + glm::vec2(-h.x, -h.y),
      center + glm::vec2(h.x, -h.y),
      center + glm::vec2(h.x, h.y),
      center + glm::vec2(-h.x, h.y),
  };
  drawPolyline(pts, color, width, true);
}

void Renderer2D::drawGrid(glm::vec2 origin, glm::vec2 size, float spacing, glm::vec3 color) {
  if (spacing <= 0.0f) return;
  for (float x = origin.x; x <= origin.x + size.x + 1e-4f; x += spacing) {
    drawLine({x, origin.y}, {x, origin.y + size.y}, color);
  }
  for (float y = origin.y; y <= origin.y + size.y + 1e-4f; y += spacing) {
    drawLine({origin.x, y}, {origin.x + size.x, y}, color);
  }
}

void Renderer2D::drawFilledPolygon(const std::vector<glm::vec2> &pts, glm::vec3 color, float alpha) {
  if (pts.size() < 3) return;
  std::vector<uint32_t> indices = Triangulation::earClip(pts);
  if (indices.empty()) return;

  glBindVertexArray(m_flatVao);
  glBindBuffer(GL_ARRAY_BUFFER, m_flatVbo);
  glBufferData(GL_ARRAY_BUFFER, pts.size() * sizeof(glm::vec2), pts.data(), GL_DYNAMIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_flatEbo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), indices.data(), GL_DYNAMIC_DRAW);

  m_flatShader.use();
  m_flatShader.setMat4("viewProj", camera.getViewProjection(getAspect()));
  m_flatShader.setVec3("uColor", color);
  m_flatShader.setFloat("uAlpha", alpha);

  glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
  glBindVertexArray(0);
}

void Renderer2D::addPoint(glm::vec2 pos, float radius, glm::vec3 color) {
  m_pointBatch.addPoint(pos, radius, color);
}

glm::vec2 Renderer2D::screenToWorld(glm::vec2 screenPx) const {
  return camera.screenToWorld(screenPx, viewportSize());
}

glm::vec2 Renderer2D::worldToScreen(glm::vec2 world) const {
  return camera.worldToScreen(world, viewportSize());
}

float Renderer2D::getAspect() const {
  return static_cast<float>(m_gui.getFramebufferWidth()) / static_cast<float>(m_gui.getFramebufferHeight());
}

glm::vec2 Renderer2D::viewportSize() const {
  return {static_cast<float>(m_gui.getWindowWidth()), static_cast<float>(m_gui.getWindowHeight())};
}
