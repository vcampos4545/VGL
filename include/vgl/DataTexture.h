#ifndef VGL_DATA_TEXTURE_H
#define VGL_DATA_TEXTURE_H

#include <GL/glew.h>

// GPU texture for uploading raw floating-point simulation data every frame
// (scalar fields, vector fields, obstacle masks). Unlike vgl::Texture (which
// loads image files via stb_image), this class re-uploads CPU-side arrays
// each frame and reuses the GL texture object/storage across uploads when
// the size doesn't change, avoiding per-frame allocations.
class DataTexture {
public:
  DataTexture() = default;
  ~DataTexture();

  DataTexture(const DataTexture &) = delete;
  DataTexture &operator=(const DataTexture &) = delete;
  DataTexture(DataTexture &&other) noexcept;
  DataTexture &operator=(DataTexture &&other) noexcept;

  // Single-channel float data, row-major, size w*h.
  void uploadR32F(const float *data, int w, int h);
  // Two-channel float data (e.g. velocity u,v), row-major, size w*h*2.
  void uploadRG32F(const float *data, int w, int h);

  void bind(int unit = 0) const;

  GLuint id() const { return m_id; }
  bool isValid() const { return m_id != 0; }
  int width() const { return m_width; }
  int height() const { return m_height; }

private:
  void ensureAllocated(int w, int h, GLenum internalFormat, GLenum format);

  GLuint m_id = 0;
  int m_width = 0;
  int m_height = 0;
  GLenum m_internalFormat = 0;
};

#endif
