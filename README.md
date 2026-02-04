# vaughngl

Simple 2D/3D shape rendering library wrapping OpenGL.

## Dependencies

```bash
# macOS
brew install glfw glew glm

# Ubuntu
sudo apt install libglfw3-dev libglew-dev libglm-dev
```

## Run Example

```bash
mkdir build && cd build
cmake ..
make
./example
```

## Use in Your Project

```cpp
#include <vaughngl/vaughngl.h>

int main() {
  GUI gui(800, 600, "My App");
  gui.camera.position = {0, 2, 8};

  while (!gui.shouldClose()) {
    gui.beginFrame();

    gui.drawSphere({0, 0, 0}, 1.0f, {1, 0, 0});
    gui.drawCube({2, 0, 0}, 1.0f, {0, 1, 0});
    gui.drawCircle({0, 2, 0}, 0.5f, {1, 1, 0});
    gui.drawRect({-2, 0, 0}, 1.0f, 0.5f, {0, 0, 1});
    gui.drawLine({-3, 0, 0}, {3, 0, 0}, {1, 1, 1});

    gui.endFrame();
  }
}
```

```cmake
# CMakeLists.txt
add_subdirectory(vaughngl)
target_link_libraries(your_app vaughngl)
```
