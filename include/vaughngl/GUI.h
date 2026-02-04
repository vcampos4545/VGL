#ifndef GUI_H
#define GUI_H

#include <vaughngl/Shader.h>
#include <vaughngl/Mesh.h>
#include <vaughngl/Camera.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <string>

class GUI {
public:
  GUI(int width, int height, const char* title = "GUI Window");
  ~GUI();

  GUI(const GUI&) = delete;
  GUI& operator=(const GUI&) = delete;

  bool shouldClose() const;
  void beginFrame();
  void endFrame();

  // 2D shapes (drawn in XY plane, can be positioned in 3D)
  void drawCircle(glm::vec3 pos, float radius, glm::vec3 color = {1, 1, 1});
  void drawRect(glm::vec3 pos, float width, float height, glm::vec3 color = {1, 1, 1});
  void drawLine(glm::vec3 start, glm::vec3 end, glm::vec3 color = {1, 1, 1}, float width = 1.0f);

  // 3D shapes
  void drawSphere(glm::vec3 pos, float radius, glm::vec3 color = {1, 1, 1});
  void drawCube(glm::vec3 pos, float size, glm::vec3 color = {1, 1, 1});
  void drawBox(glm::vec3 pos, glm::vec3 size, glm::vec3 color = {1, 1, 1});

  // Lighting control
  void setLighting(bool enabled) { m_useLighting = enabled; }
  void setLightDirection(glm::vec3 dir) { m_lightDir = glm::normalize(dir); }

  Camera camera;

  int getWidth() const { return m_width; }
  int getHeight() const { return m_height; }
  GLFWwindow* getWindow() const { return m_window; }

private:
  void initGL();
  void initMeshes();
  void setupDraw(const glm::mat4& model, glm::vec3 color);

  GLFWwindow* m_window = nullptr;
  int m_width, m_height;

  Shader m_shader;
  Mesh m_circleMesh;
  Mesh m_quadMesh;
  Mesh m_cubeMesh;
  Mesh m_sphereMesh;
  Mesh m_lineMesh;

  bool m_useLighting = true;
  glm::vec3 m_lightDir{0.5f, 1.0f, 0.3f};
};

#endif
