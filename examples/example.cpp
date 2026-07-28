// Demo / smoke test for VGL's 3D immediate-mode module. Deliberately built as
// a small still-life scene rather than a bare feature checklist: a lit floor,
// primitives with varied surface reflectivity, an OBJ mesh with its own
// per-face materials, a spinning object, an unlit marker, and a vector arrow
// -- composed to also work as a single representative screenshot of the
// library, not just something you have to run to appreciate.
#include <vgl/vgl.h>
#include <cmath>

int main()
{
  GUI gui(1280, 800, "VGL Demo");

  gui.camera.position = {0.0f, 3.4f, 9.0f};
  gui.camera.target = {0.0f, 0.8f, 0.0f};

  // A warm low-angle key light with a cool sky / warm ground hemisphere
  // ambient fill -- see setLightColor/setLightIntensity/setAmbientLight in
  // GUI.h. Without the hemisphere fill, anything facing away from the key
  // light would go flat dark; the sky/ground blend gives it a believable
  // bounce instead.
  gui.setLightDirection({0.5f, 1.0f, 0.35f});
  gui.setLightColor({1.0f, 0.92f, 0.78f});
  gui.setLightIntensity(1.25f);
  gui.setAmbientLight(/*sky*/ {0.28f, 0.32f, 0.42f}, /*ground*/ {0.10f, 0.09f, 0.08f});
  gui.setClearColor({0.06f, 0.07f, 0.10f});

  glm::vec2 lastMousePos = gui.getMousePosition();
  glm::vec3 cubePos = {0.0f, 0.7f, 0.0f};
  float moveSpeed = 0.1f;

  glm::quat spinRotation = glm::quat(1, 0, 0, 0); // identity quaternion

  OBJMesh pyramid;
  if (!pyramid.load("models/pyramid.obj"))
  {
    return 1;
  }

  // Surface reflectivity presets -- same geometry, different Material, to
  // show ambient/specular/shininess actually changing how light behaves
  // per object rather than every shape sharing one hardcoded look.
  Material glossy{.ambient = {1, 1, 1}, .specular = {0.9f, 0.9f, 0.9f}, .shininess = 96.0f};
  Material matte{.ambient = {1, 1, 1}, .specular = {0.05f, 0.05f, 0.05f}, .shininess = 8.0f};
  Material satin{.ambient = {1, 1, 1}, .specular = {0.4f, 0.4f, 0.4f}, .shininess = 32.0f};

  while (!gui.shouldClose())
  {
    gui.beginFrame();

    // Orbital camera - drag to rotate around target
    glm::vec2 mousePos = gui.getMousePosition();
    if (gui.isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
    {
      glm::vec2 delta = mousePos - lastMousePos;
      float sensitivity = 0.005f;

      glm::vec3 offset = gui.camera.position - gui.camera.target;
      float radius = glm::length(offset);
      float theta = std::atan2(offset.x, offset.z);
      float phi = std::acos(offset.y / radius);

      theta -= delta.x * sensitivity;
      phi -= delta.y * sensitivity;
      phi = glm::clamp(phi, 0.1f, 3.04f);

      gui.camera.position.x = gui.camera.target.x + radius * std::sin(phi) * std::sin(theta);
      gui.camera.position.y = gui.camera.target.y + radius * std::cos(phi);
      gui.camera.position.z = gui.camera.target.z + radius * std::sin(phi) * std::cos(theta);
    }
    lastMousePos = mousePos;

    // Scroll to zoom camera
    glm::vec2 scroll = gui.getScrollDelta();
    if (scroll.y != 0.0f)
    {
      glm::vec3 direction = glm::normalize(gui.camera.position - gui.camera.target);
      gui.camera.position -= direction * scroll.y * 0.5f;
    }

    // Move the green cube with arrow keys
    if (gui.isKeyPressed(GLFW_KEY_UP))
      cubePos.z -= moveSpeed;
    if (gui.isKeyPressed(GLFW_KEY_DOWN))
      cubePos.z += moveSpeed;
    if (gui.isKeyPressed(GLFW_KEY_LEFT))
      cubePos.x -= moveSpeed;
    if (gui.isKeyPressed(GLFW_KEY_RIGHT))
      cubePos.x += moveSpeed;

    glm::quat spin = glm::angleAxis(0.02f, glm::normalize(glm::vec3(1, 1, 0)));
    spinRotation = spin * spinRotation;

    // Floor -- a large flat, lit box rather than the ray-cast infinite
    // ground plane (drawInfiniteGroundPlane), since that feature assumes a
    // Z-up world and this scene is Y-up like the rest of the 3D API.
    gui.drawBox({0.0f, -0.05f, 0.0f}, {14.0f, 0.1f, 14.0f}, {0.16f, 0.15f, 0.17f}, satin);

    // Same three shape types, three different surface finishes.
    gui.drawSphere({-3.2f, 0.9f, -1.0f}, 0.9f, {0.85f, 0.15f, 0.15f}, glossy);
    gui.drawBox({3.2f, 0.9f, -1.2f}, {0.7f, 1.8f, 0.7f}, {0.30f, 0.40f, 0.90f}, satin);
    gui.drawCylinder({-1.6f, 1.0f, 2.0f}, 0.35f, 2.0f, {0.75f, 0.55f, 0.90f}, satin);

    // Spinning cube, movable with arrow keys -- quaternion rotation + matte material.
    gui.drawCube(cubePos, 1.4f, spinRotation, {0.25f, 0.75f, 0.35f}, matte);

    // OBJ mesh, using its own per-submesh materials parsed from the .mtl file.
    gui.drawOBJMesh(pyramid, {2.2f, 0.0f, 1.8f}, 1.4f, spinRotation);

    // Unlit marker -- Material::Unlit() opts an individual object out of
    // scene lighting entirely, for a flat "always visible" gizmo/beacon look.
    gui.drawSphere({0.0f, 3.4f, 0.0f}, 0.15f, {1.0f, 0.95f, 0.30f}, Material::Unlit());

    // A vector arrow, same batched-line machinery the 2D module uses for
    // vector fields, marking the beacon above the red sphere.
    gui.drawArrow({-3.2f, 1.9f, -1.0f}, {-3.2f, 2.7f, -1.0f}, {1.0f, 1.0f, 1.0f}, 2.0f);

    gui.endFrame();
  }

  return 0;
}
