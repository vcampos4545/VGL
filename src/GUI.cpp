#include <vaughngl/GUI.h>
#include <stdexcept>
#include <cstdio>
#include <glm/gtc/matrix_transform.hpp>

GUI::GUI(int width, int height, const char* title)
    : m_width(width), m_height(height) {
  initGL();

  m_window = glfwCreateWindow(width, height, title, nullptr, nullptr);
  if (!m_window) {
    glfwTerminate();
    throw std::runtime_error("Failed to create window");
  }

  glfwMakeContextCurrent(m_window);

  if (glewInit() != GLEW_OK) {
    throw std::runtime_error("Failed to initialize GLEW");
  }

  glViewport(0, 0, width, height);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  m_shader.load("shaders/default.vert", "shaders/default.frag");
  initMeshes();
}

GUI::~GUI() {
  if (m_window) glfwDestroyWindow(m_window);
  glfwTerminate();
}

void GUI::initGL() {
  if (!glfwInit()) {
    throw std::runtime_error("Failed to initialize GLFW");
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
}

void GUI::initMeshes() {
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;

  MeshGen::circle(vertices, indices, 32);
  m_circleMesh.upload(vertices, indices);

  MeshGen::quad(vertices, indices);
  m_quadMesh.upload(vertices, indices);

  MeshGen::cube(vertices, indices);
  m_cubeMesh.upload(vertices, indices);

  MeshGen::sphere(vertices, indices, 16, 32);
  m_sphereMesh.upload(vertices, indices);
}

bool GUI::shouldClose() const {
  return glfwWindowShouldClose(m_window);
}

void GUI::beginFrame() {
  glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  m_shader.use();

  float aspect = (float)m_width / m_height;
  m_shader.setMat4("view", camera.getViewMatrix());
  m_shader.setMat4("projection", camera.getProjectionMatrix(aspect));
  m_shader.setBool("useLighting", m_useLighting);
  m_shader.setVec3("lightDir", m_lightDir);
  m_shader.setVec3("viewPos", camera.position);
}

void GUI::endFrame() {
  glfwSwapBuffers(m_window);
  glfwPollEvents();
}

void GUI::setupDraw(const glm::mat4& model, glm::vec3 color) {
  m_shader.setMat4("model", model);
  m_shader.setVec3("color", color);
}

void GUI::drawCircle(glm::vec3 pos, float radius, glm::vec3 color) {
  glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
  model = glm::scale(model, glm::vec3(radius));

  bool prevLighting = m_useLighting;
  m_shader.setBool("useLighting", false);
  setupDraw(model, color);
  m_circleMesh.draw();
  m_shader.setBool("useLighting", prevLighting);
}

void GUI::drawRect(glm::vec3 pos, float width, float height, glm::vec3 color) {
  glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
  model = glm::scale(model, glm::vec3(width, height, 1.0f));

  bool prevLighting = m_useLighting;
  m_shader.setBool("useLighting", false);
  setupDraw(model, color);
  m_quadMesh.draw();
  m_shader.setBool("useLighting", prevLighting);
}

void GUI::drawLine(glm::vec3 start, glm::vec3 end, glm::vec3 color, float width) {
  std::vector<glm::vec3> points = {start, end};
  m_lineMesh.uploadLines(points);

  glLineWidth(width);
  m_shader.setBool("useLighting", false);
  setupDraw(glm::mat4(1.0f), color);
  m_lineMesh.drawLines();
  m_shader.setBool("useLighting", m_useLighting);
}

void GUI::drawSphere(glm::vec3 pos, float radius, glm::vec3 color) {
  glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
  model = glm::scale(model, glm::vec3(radius * 2.0f)); // mesh is unit diameter

  setupDraw(model, color);
  m_sphereMesh.draw();
}

void GUI::drawCube(glm::vec3 pos, float size, glm::vec3 color) {
  drawBox(pos, glm::vec3(size), color);
}

void GUI::drawBox(glm::vec3 pos, glm::vec3 size, glm::vec3 color) {
  glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
  model = glm::scale(model, size);

  setupDraw(model, color);
  m_cubeMesh.draw();
}
