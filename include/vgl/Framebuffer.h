#ifndef VGL_FRAMEBUFFER_H
#define VGL_FRAMEBUFFER_H

#include <GL/glew.h>

// Offscreen render target with a single color attachment (plus depth/stencil
// renderbuffer) and no MSAA. Intended for render-to-texture visualization
// panels (e.g. rendering the simulation into a texture shown inside some
// other UI window) and simple post-processing.
class Framebuffer {
public:
  Framebuffer() = default;
  ~Framebuffer();

  Framebuffer(const Framebuffer &) = delete;
  Framebuffer &operator=(const Framebuffer &) = delete;
  Framebuffer(Framebuffer &&other) noexcept;
  Framebuffer &operator=(Framebuffer &&other) noexcept;

  // floatingPoint: use a 16-bit float color attachment (GL_RGBA16F) instead
  // of GL_RGBA8. Useful when rendering HDR-ish data that will be sampled
  // again rather than displayed directly.
  void create(int width, int height, bool floatingPoint = false);
  void resize(int width, int height);

  void bind() const;
  static void unbind();

  GLuint colorTexture() const { return m_colorTex; }
  GLuint handle() const { return m_fbo; }
  int width() const { return m_width; }
  int height() const { return m_height; }
  bool isValid() const { return m_fbo != 0; }

private:
  void destroy();

  GLuint m_fbo = 0;
  GLuint m_colorTex = 0;
  GLuint m_depthRbo = 0;
  int m_width = 0;
  int m_height = 0;
  bool m_float = false;
};

#endif
