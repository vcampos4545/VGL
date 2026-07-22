# VGL

A minimal OpenGL rendering layer for simulation and visualization tools.

<img width="798" height="592" alt="Image" src="https://github.com/user-attachments/assets/7702ea99-ebd3-449a-a2e8-f514348da2b5" />

## Dependencies

```bash
# macOS
brew install glfw glew glm

# Ubuntu
sudo apt install libglfw3-dev libglew-dev libglm-dev
```

## Use in Your Project

### CMake FetchContent (Recommended)

```cmake
include(FetchContent)

FetchContent_Declare(
  vgl
  GIT_REPOSITORY https://github.com/vcampos4545/VGL.git
  GIT_TAG main
)
FetchContent_MakeAvailable(vgl)

target_link_libraries(your_app vgl)
```

### Git Submodule

```bash
git submodule add https://github.com/vcampos4545/VGL.git external/vgl
```

```cmake
add_subdirectory(external/vgl)
target_link_libraries(your_app vgl)
```

## Quick Start

```cpp
#include <vgl/vgl.h>

int main() {
  GUI gui(800, 600, "My App");

  gui.camera.setPosition({0, 2, 8})
            .setTarget({0, 0, 0})
            .setFOV(45.0f);

  while (!gui.shouldClose()) {
    gui.beginFrame();

    // 3D shapes
    gui.drawSphere({0, 0, 0}, 1.0f, {1, 0, 0});
    gui.drawCube({2, 0, 0}, 1.0f, {0, 1, 0});
    gui.drawBox({-2, 0, 0}, {1, 2, 0.5f}, {0, 0, 1});

    // 2D shapes (in 3D space)
    gui.drawCircle({0, 2, 0}, 0.5f, {1, 1, 0});
    gui.drawRect({0, 3, 0}, 1.0f, 0.5f, {0, 1, 1});
    gui.drawLine({-3, 0, 0}, {3, 0, 0}, {1, 1, 1});

    gui.endFrame();
  }
}
```

### Rotation with Quaternions

```cpp
glm::quat rotation = glm::angleAxis(glm::radians(45.0f), glm::vec3(0, 1, 0));
gui.drawCube({0, 0, 0}, 1.0f, rotation, {1, 0, 0});
gui.drawSphere({2, 0, 0}, 0.5f, rotation, {0, 1, 0});
gui.drawBox({-2, 0, 0}, {1, 2, 1}, rotation, {0, 0, 1});
```

### OBJ Model Loading

```cpp
OBJMesh model;
if (model.load("models/spaceship.obj")) {
  // Draw with material colors from .mtl file
  gui.drawOBJMesh(model, {0, 0, 0}, 1.0f);

  // Draw with rotation
  gui.drawOBJMesh(model, {0, 0, 0}, 1.0f, rotation);

  // Draw with color override
  gui.drawOBJMesh(model, {0, 0, 0}, 1.0f, {1, 0, 0});

  // Non-uniform scale
  gui.drawOBJMesh(model, {0, 0, 0}, glm::vec3(1, 2, 1));
}
```

### Input Handling

```cpp
// Keyboard
if (gui.isKeyPressed(GLFW_KEY_W)) { /* held down */ }
if (gui.isKeyJustPressed(GLFW_KEY_SPACE)) { /* pressed this frame */ }
if (gui.isKeyJustReleased(GLFW_KEY_ESCAPE)) { /* released this frame */ }

// Mouse
glm::vec2 mousePos = gui.getMousePosition();
if (gui.isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) { /* held */ }
if (gui.isMouseButtonJustPressed(GLFW_MOUSE_BUTTON_RIGHT)) { /* clicked */ }

// Scroll wheel
glm::vec2 scroll = gui.getScrollDelta();
```

### Camera Control

```cpp
// Chainable setters
gui.camera.setPosition({0, 5, 10})
          .setTarget({0, 0, 0})
          .setFOV(60.0f)
          .setClipPlanes(0.1f, 1000.0f);

// Or set directly
gui.camera.position = {0, 5, 10};
gui.camera.target = {0, 0, 0};
gui.camera.fov = 60.0f;
gui.camera.nearPlane = 0.1f;
gui.camera.farPlane = 1000.0f;

// Utility methods
glm::vec3 dir = gui.camera.getDirection();
float dist = gui.camera.getDistance();
gui.camera.setDistance(20.0f);
```

### Lighting

```cpp
gui.setLighting(true);  // Enable/disable
gui.setLightDirection({1, 1, 0.5f});
```

## 2D Rendering Module

`GUI` above is built around 3D scenes. For 2D/orthographic work (CFD
visualization, editors, dashboards, HUDs) use `Renderer2D`, built on top
of a `GUI`'s window/context/input:

```cpp
#include <vgl/vgl.h>

GUI gui(900, 700, "2D App");
Renderer2D renderer(gui);
renderer.camera.setCenter({0, 0}).setViewHeight(4.0f);

DataTexture field;
field.uploadR32F(scalarData.data(), width, height); // re-upload any frame; reuses GPU storage

while (!gui.shouldClose()) {
  renderer.beginFrame({0.08f, 0.08f, 0.1f});

  renderer.drawScalarField(field, {-2, -2}, {4, 4}, /*vmin*/ -1, /*vmax*/ 1, Colormap::Viridis);
  renderer.drawGrid({-2, -2}, {4, 4}, 0.5f, {0.3f, 0.3f, 0.3f});
  renderer.drawArrow({0, 0}, {1, 0.5f}, {1, 0.3f, 0.2f}, 2.0f);
  renderer.drawFilledPolygon({{0,0}, {1,0}, {0.5f,1}}, {0.2f, 0.7f, 1.0f});
  for (int i = 0; i < 500; ++i) renderer.addPoint(particlePos[i], 0.02f, {1, 0.8f, 0.2f});

  renderer.endFrame();
}
```

- **`Camera2D`** — orthographic, pan/zoom via `center`/`viewHeight`,
  `screenToWorld`/`worldToScreen` for picking.
- **`DataTexture`** — upload raw float arrays (`uploadR32F`/`uploadRG32F`)
  every frame without reallocating; the vehicle for scalar/vector field
  visualization.
- **`drawScalarField`** — a world-space rect textured with a `DataTexture`
  through a selectable `Colormap` (Viridis/Jet/CoolWarm/Grayscale/Fire),
  with optional contour isolines and a second `DataTexture` composited as
  a solid/obstacle mask.
- **Batched primitives** — `drawLine`/`drawPolyline`/`drawArrow`/
  `drawCircleOutline`/`drawRectOutline`/`drawGrid` all accumulate into one
  `LineBatch2D` and draw in a single `GL_LINES` call per frame.
  `drawFilledPolygon` triangulates (ear-clipping, `Triangulation.h`,
  handles concave polygons) and draws immediately — fine for the
  low-frequency case (object outlines), not meant for per-cell fills.
- **`addPoint`** — instanced circle rendering (`PointBatch2D`) for
  particle systems; thousands of points, one draw call.
- **`Framebuffer`** — offscreen color+depth/stencil target for
  render-to-texture panels or post-processing, independent of the 2D
  module.

If you're layering an immediate-mode UI (Dear ImGui, etc.) on top of a
`Renderer2D` scene, don't call `endFrame()` directly — its buffer swap
needs to happen *after* the UI's own render pass, or the UI will be
drawn one frame behind the scene:

```cpp
renderer.beginFrame();
// ... draw the 2D scene ...
renderer.flush();          // draws the accumulated line/point batches, no swap
// ... run the UI library's own render pass here ...
renderer.present();        // swaps buffers, polls events
```

`endFrame()` is just `flush() + present()` and remains correct for a
plain 2D scene with nothing composited on top.

## Run Example

```bash
mkdir build && cd build
cmake ..
make
./example      # 3D orbit-camera demo
./example2d    # 2D module demo (scalar field, lines, filled polygon, particles)
```

### Example Controls

- **Left mouse drag**: Orbit camera
- **Scroll**: Zoom in/out
- **Arrow keys**: Move the green cube
