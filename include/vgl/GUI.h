#ifndef GUI_H
#define GUI_H

#include <vgl/Shader.h>
#include <vgl/Mesh.h>
#include <vgl/Camera.h>
#include <vgl/Material.h>
#include <vgl/OBJMesh.h>
#include <vgl/Texture.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <unordered_set>

class GUI
{
public:
  GUI(int width, int height, const char *title = "GUI Window");
  ~GUI();

  GUI(const GUI &) = delete;
  GUI &operator=(const GUI &) = delete;

  bool shouldClose() const;
  void beginFrame();
  void endFrame();

  // 2D shapes (drawn in XY plane, can be positioned in 3D). Unlit by default
  // (matching prior behavior) since a flat facing-camera shape gains little
  // from lighting, but pass a Material with lit = true to opt in.
  void drawCircle(glm::vec3 pos, float radius, glm::vec3 color = {1, 1, 1}, Material material = Material::Unlit());
  void drawCircle(glm::vec3 pos, float radius, glm::quat rotation, glm::vec3 color = {1, 1, 1}, Material material = Material::Unlit());
  void drawRect(glm::vec3 pos, float width, float height, glm::vec3 color = {1, 1, 1}, Material material = Material::Unlit());
  void drawRect(glm::vec3 pos, float width, float height, glm::quat rotation, glm::vec3 color = {1, 1, 1}, Material material = Material::Unlit());
  void drawLine(glm::vec3 start, glm::vec3 end, glm::vec3 color = {1, 1, 1}, float width = 1.0f);
  void drawArrow(glm::vec3 start, glm::vec3 end, glm::vec3 color = {1, 1, 1}, float width = 1.0f);

  // 3D shapes. Lit by default, using the current scene light + the given
  // Material's reflectivity (default Material is a reasonable neutral
  // surface); pass Material with lit = false for a flat unlit color.
  void drawSphere(glm::vec3 pos, float radius, glm::vec3 color = {1, 1, 1}, Material material = {});
  void drawSphere(glm::vec3 pos, float radius, glm::quat rotation, glm::vec3 color = {1, 1, 1}, Material material = {});
  void drawCube(glm::vec3 pos, float size, glm::vec3 color = {1, 1, 1}, Material material = {});
  void drawCube(glm::vec3 pos, float size, glm::quat rotation, glm::vec3 color = {1, 1, 1}, Material material = {});
  void drawBox(glm::vec3 pos, glm::vec3 size, glm::vec3 color = {1, 1, 1}, Material material = {});
  void drawBox(glm::vec3 pos, glm::vec3 size, glm::quat rotation, glm::vec3 color = {1, 1, 1}, Material material = {});
  void drawCylinder(glm::vec3 pos, float radius, float length, glm::vec3 color = {1, 1, 1}, Material material = {});
  void drawCylinder(glm::vec3 pos, float radius, float length, glm::quat rotation, glm::vec3 color = {1, 1, 1}, Material material = {});
  void drawCylinder(glm::vec3 pos, float radius, float length, glm::vec3 axis, glm::quat rotation, glm::vec3 color = {1, 1, 1}, Material material = {});

  // Textured sphere — lit version matches drawSphere lighting; pass a rotation
  // quaternion to orient the texture (e.g. Earth rotation around Z axis).
  void drawTexturedSphere(glm::vec3 pos, float radius, const Texture& texture, Material material = {});
  void drawTexturedSphere(glm::vec3 pos, float radius, glm::quat rotation, const Texture& texture, Material material = {});

  // Draw a texture-mapped sphere that fills the background (star field, sky).
  // Translation is stripped from the view so the background stays fixed as the
  // camera moves; depth writes are disabled so it never occludes scene objects.
  void drawBackground(const Texture& texture);

  // A true infinite ground plane (world-space plane z = planeZ), rendered
  // by ray-casting per pixel in the fragment shader rather than as a mesh
  // -- see EmbeddedShaders::groundPlaneFrag. Fades to fadeColor (typically
  // your clear/sky color) at maxDistance. Unlike a giant flat box, this has
  // no floating-point/depth precision breakdown far from the origin, since
  // there are no vertices out there to lose precision on.
  void drawInfiniteGroundPlane(glm::vec3 groundColor, glm::vec3 fadeColor,
                               float planeZ = 0.0f, float maxDistance = 100000.0f);

  // OBJ mesh drawing (uses material colors from the mesh)
  void drawOBJMesh(OBJMesh &mesh, glm::vec3 pos, float scale = 1.0f);
  void drawOBJMesh(OBJMesh &mesh, glm::vec3 pos, float scale, glm::quat rotation);
  void drawOBJMesh(OBJMesh &mesh, glm::vec3 pos, glm::vec3 scale);
  void drawOBJMesh(OBJMesh &mesh, glm::vec3 pos, glm::vec3 scale, glm::quat rotation);
  // OBJ mesh with color override (uses the given color instead of each
  // submesh's diffuse color, but still applies its parsed ambient/specular/
  // shininess/lit reflectivity properties)
  void drawOBJMesh(OBJMesh &mesh, glm::vec3 pos, float scale, glm::vec3 color);
  void drawOBJMesh(OBJMesh &mesh, glm::vec3 pos, float scale, glm::quat rotation, glm::vec3 color);
  void drawOBJMesh(OBJMesh &mesh, glm::vec3 pos, glm::vec3 scale, glm::vec3 color);
  void drawOBJMesh(OBJMesh &mesh, glm::vec3 pos, glm::vec3 scale, glm::quat rotation, glm::vec3 color);

  // Lighting control
  // Master switch: false disables lighting scene-wide regardless of any
  // per-object Material's lit flag (e.g. for a fully flat-shaded style).
  void setLighting(bool enabled) { m_useLighting = enabled; }
  void setLightDirection(glm::vec3 dir) { m_lightDir = glm::normalize(dir); }
  void setLightColor(glm::vec3 color) { m_lightColor = color; }
  void setLightIntensity(float intensity) { m_lightIntensity = intensity; }
  // Hemisphere ambient fill: sky is blended in on up-facing surfaces, ground
  // on down-facing ones, so faces angled away from the main light don't go
  // flat dark. A per-object Material's ambient field multiplies this.
  void setAmbientLight(glm::vec3 skyColor, glm::vec3 groundColor) {
    m_ambientSky = skyColor;
    m_ambientGround = groundColor;
  }

  // Background (glClearColor) shown wherever nothing is drawn. Default is a
  // neutral dark grey; scenes can set this per their setting (deep space,
  // an overcast sky, etc).
  void setClearColor(glm::vec3 color) { m_clearColor = color; }

  // Logarithmic depth buffer — eliminates z-fighting over huge depth ranges.
  // Pass farPlane (same value as camera.farPlane) to enable; 0 = disabled (default).
  void setLogDepth(float farPlane) { m_logDepthFarPlane = farPlane; }

  // Override the aspect ratio used to build the projection matrix.
  // Use this when rendering into a sub-viewport (e.g. a split-screen panel).
  // Pass 0 to restore automatic aspect from the framebuffer dimensions.
  void setAspectOverride(float aspect) { m_aspectOverride = aspect; }
  void clearAspectOverride()           { m_aspectOverride = 0.0f;   }

  // Keyboard input
  bool isKeyPressed(int key) const;
  bool isKeyJustPressed(int key) const;
  bool isKeyJustReleased(int key) const;

  // Mouse input
  glm::vec2 getMousePosition() const;
  bool isMouseButtonPressed(int button) const;
  bool isMouseButtonJustPressed(int button) const;
  bool isMouseButtonJustReleased(int button) const;
  glm::vec2 getScrollDelta() const;

  // Raycasting: unproject a mouse position into a world-space ray direction
  glm::vec3 getMouseRay(glm::vec2 mousePos) const;

  Camera camera;

  int getWindowWidth() const { return m_windowWidth; }
  int getWindowHeight() const { return m_windowHeight; }

  int getFramebufferWidth() const { return m_framebufferWidth; }
  int getFramebufferHeight() const { return m_framebufferHeight; }
  float getAspect() const { return static_cast<float>(m_framebufferWidth) / m_framebufferHeight; }
  GLFWwindow *getWindow() const { return m_window; }

private:
  void initGL();
  void initMeshes();
  void setupCallbacks();
  void setupDraw(const glm::mat4 &model, glm::vec3 color, const Material &material);

  // GLFW callbacks
  static void framebufferSizeCallback(GLFWwindow *window, int width, int height);
  static void windowSizeCallback(GLFWwindow *window, int width, int height);
  static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
  static void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
  static void scrollCallback(GLFWwindow *window, double xoffset, double yoffset);

  GLFWwindow *m_window = nullptr;
  int m_windowWidth;
  int m_windowHeight;

  int m_framebufferWidth;
  int m_framebufferHeight;

  Shader m_shader;
  Shader m_groundPlaneShader;
  Mesh m_circleMesh;
  Mesh m_quadMesh;
  Mesh m_cubeMesh;
  Mesh m_sphereMesh;
  Mesh m_cylinderMesh;
  Mesh m_lineMesh;

  bool m_useLighting = true;
  glm::vec3 m_lightDir{0.5f, 1.0f, 0.3f};
  glm::vec3 m_lightColor{1.0f, 1.0f, 1.0f};
  float m_lightIntensity = 1.0f;
  glm::vec3 m_ambientSky{0.30f, 0.32f, 0.38f};
  glm::vec3 m_ambientGround{0.12f, 0.11f, 0.10f};
  glm::vec3 m_clearColor{0.1f, 0.1f, 0.1f};
  float m_logDepthFarPlane = 0.0f;
  float m_aspectOverride   = 0.0f;

  // Input state
  std::unordered_set<int> m_keysPressed;
  std::unordered_set<int> m_keysJustPressed;
  std::unordered_set<int> m_keysJustReleased;
  std::unordered_set<int> m_mouseButtonsPressed;
  std::unordered_set<int> m_mouseButtonsJustPressed;
  std::unordered_set<int> m_mouseButtonsJustReleased;
  glm::vec2 m_scrollDelta{0.0f, 0.0f};
};

#endif
