#ifndef VGL_RENDERER2D_H
#define VGL_RENDERER2D_H

#include <vgl/GUI.h>
#include <vgl/Camera2D.h>
#include <vgl/DataTexture.h>
#include <vgl/LineBatch2D.h>
#include <vgl/PointBatch2D.h>
#include <vgl/Shader.h>
#include <vgl/Mesh.h>
#include <glm/glm.hpp>
#include <vector>

enum class Colormap { Viridis = 0, Jet = 1, CoolWarm = 2, Grayscale = 3, Fire = 4 };

// 2D rendering façade built on top of a GUI's window/context/input.
// Provides everything a CFD-style visualization app needs that VGL's
// primarily-3D GUI class does not: an orthographic camera, GPU colormap
// rendering of scalar fields, and batched line/point rendering for vector
// fields, streamlines, polygons, and particles.
//
// Usage: construct once with a live GUI, then each frame call
// beginFrame() / ...draw calls... / endFrame(). Depth testing is disabled
// for the lifetime of the Renderer2D (this facade assumes a purely 2D
// scene); construct a separate GUI if 3D content is also needed.
//
// If you need to composite something else on top of the scene before the
// buffer swap — an immediate-mode UI library's own draw pass, most
// commonly — call flush() instead of endFrame(), issue that library's
// render call, then call present(). endFrame() is exactly
// flush() + present() and remains the right call for a plain 2D scene
// with nothing layered on top.
class Renderer2D {
public:
  explicit Renderer2D(GUI &gui);

  Camera2D camera;

  void beginFrame(glm::vec3 clearColor = {0.05f, 0.05f, 0.07f});

  // Draws the accumulated line/point batches. Leaves the framebuffer
  // un-swapped and GLFW events un-polled so more can be composited on top.
  void flush();

  // Swaps buffers, polls events, and clears per-frame input state
  // (via GUI::endFrame()). Call after flush() (directly, or via the
  // combined endFrame()) once nothing more will be drawn this frame.
  void present();

  void endFrame(); // flush() + present()

  // Renders a scalar field as a world-space rectangle [origin, origin+size]
  // through the given colormap, mapping values in [vmin, vmax] to [0, 1].
  // solidMask (if provided) is composited on top using solidColor where
  // mask value > 0 (fractional values blend), for obstacle visualization.
  void drawScalarField(const DataTexture &field, glm::vec2 origin, glm::vec2 size,
                        float vmin, float vmax, Colormap map,
                        const DataTexture *solidMask = nullptr,
                        bool contours = false, int contourCount = 12,
                        glm::vec3 solidColor = {0.25f, 0.25f, 0.28f});

  // Batched primitives (accumulated, drawn once in endFrame).
  void drawLine(glm::vec2 a, glm::vec2 b, glm::vec3 color, float width = 1.0f);
  void drawPolyline(const std::vector<glm::vec2> &pts, glm::vec3 color, float width = 1.0f, bool closed = false);
  void drawArrow(glm::vec2 from, glm::vec2 to, glm::vec3 color, float width = 1.0f, float headSize = 0.15f);
  void drawCircleOutline(glm::vec2 pos, float radius, glm::vec3 color, float width = 1.0f, int segments = 32);
  void drawRectOutline(glm::vec2 center, glm::vec2 size, glm::vec3 color, float width = 1.0f);
  void drawGrid(glm::vec2 origin, glm::vec2 size, float spacing, glm::vec3 color);

  // Immediate (non-batched) filled polygon; low frequency (object outlines,
  // not per-cell), so a dedicated draw call per call is fine.
  void drawFilledPolygon(const std::vector<glm::vec2> &pts, glm::vec3 color, float alpha = 1.0f);

  // Particle / point rendering (batched, instanced).
  void addPoint(glm::vec2 pos, float radius, glm::vec3 color);

  glm::vec2 screenToWorld(glm::vec2 screenPx) const;
  glm::vec2 worldToScreen(glm::vec2 world) const;
  float getAspect() const;
  glm::vec2 viewportSize() const;

  GUI &gui() { return m_gui; }

private:
  GUI &m_gui;
  Shader m_fieldShader;
  Shader m_flatShader;
  Mesh m_quadMesh;
  LineBatch2D m_lineBatch;
  PointBatch2D m_pointBatch;
  float m_lineWidth = 1.0f;

  GLuint m_flatVao = 0;
  GLuint m_flatVbo = 0;
  GLuint m_flatEbo = 0;
};

#endif
