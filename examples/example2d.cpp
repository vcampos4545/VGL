// Smoke test / demo for VGL's 2D rendering module: scalar field colormap,
// batched lines/arrows, filled polygon, and instanced particle points.
#include <vgl/vgl.h>
#include <vector>
#include <cmath>

int main() {
  GUI gui(900, 700, "VGL 2D Module Demo");
  glfwSwapInterval(1);

  Renderer2D renderer(gui);
  renderer.camera.setCenter({0.0f, 0.0f}).setViewHeight(4.0f);

  const int W = 64, H = 64;
  std::vector<float> field(W * H);
  std::vector<float> mask(W * H, 0.0f);
  for (int j = 0; j < H; ++j) {
    for (int i = 0; i < W; ++i) {
      float x = (i / float(W - 1)) * 2.0f - 1.0f;
      float y = (j / float(H - 1)) * 2.0f - 1.0f;
      field[j * W + i] = std::sin(x * 5.0f) * std::cos(y * 5.0f);
      float d = std::sqrt(x * x + y * y);
      mask[j * W + i] = d < 0.15f ? 1.0f : 0.0f;
    }
  }

  DataTexture fieldTex;
  fieldTex.uploadR32F(field.data(), W, H);
  DataTexture maskTex2;
  maskTex2.uploadR32F(mask.data(), W, H);

  float t = 0.0f;
  int frame = 0;
  while (!gui.shouldClose() && frame < 600) {
    renderer.beginFrame({0.08f, 0.08f, 0.1f});

    renderer.drawScalarField(fieldTex, {-2.0f, -2.0f}, {4.0f, 4.0f}, -1.0f, 1.0f,
                              Colormap::Viridis, &maskTex2, true, 10);

    renderer.drawGrid({-2.0f, -2.0f}, {4.0f, 4.0f}, 0.5f, {0.3f, 0.3f, 0.3f});
    renderer.drawCircleOutline({0, 0}, 0.3f + 0.05f * std::sin(t), {1, 1, 1}, 2.0f);
    renderer.drawArrow({-1.5f, 1.5f}, {-0.5f, 1.5f + 0.3f * std::sin(t)}, {1, 0.3f, 0.2f}, 2.0f);
    renderer.drawFilledPolygon({{1.0f, -1.5f}, {1.6f, -1.5f}, {1.6f, -1.0f}, {1.3f, -0.7f}, {1.0f, -1.0f}},
                                {0.2f, 0.7f, 1.0f}, 0.85f);

    for (int i = 0; i < 200; ++i) {
      float a = i * 0.15f + t;
      renderer.addPoint({1.5f * std::cos(a) * 0.6f, 1.5f * std::sin(a) * 0.6f + 1.0f}, 0.02f,
                         {1.0f, 0.8f, 0.2f});
    }

    renderer.endFrame();
    t += 0.03f;
    ++frame;
  }

  return 0;
}
