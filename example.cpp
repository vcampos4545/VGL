#include <vaughngl/vaughngl.h>

int main()
{
  GUI gui(800, 600, "Vaughn-GL Demo");

  gui.camera.position = {0, 2, 8};
  gui.camera.target = {0, 0, 0};

  while (!gui.shouldClose())
  {
    gui.beginFrame();

    // Draw grid lines
    int numLines = 10;
    for (int i = 0; i < numLines + 1; ++i)
    {
      // Horizontal
      gui.drawLine({-numLines / 2, 0, i - numLines / 2}, {numLines / 2, 0, i - numLines / 2}, {1, 1, 1}, 2.0f);
      // Vertical
      gui.drawLine({i - numLines / 2, 0, numLines / 2}, {i - numLines / 2, 0, -numLines / 2}, {1, 1, 1}, 2.0f);
    }

    // 3D shapes with lighting
    gui.drawSphere({-5, 0, -5}, 0.8f, {1, 0.3f, 0.3f});
    gui.drawCube({0, 1, 0}, 1.2f, {0.3f, 1, 0.3f});
    gui.drawBox({5, 0, -5}, {0.5f, 1.5f, 0.5f}, {0.3f, 0.3f, 1});

    gui.endFrame();
  }

  return 0;
}