#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>
#include <string>

// 2D texture loaded from a file (JPEG, PNG, etc.) via stb_image.
// Call loadFromFile() after the OpenGL context is live.
class Texture {
public:
  Texture() = default;
  ~Texture();

  Texture(Texture&& other) noexcept;
  Texture& operator=(Texture&& other) noexcept;
  Texture(const Texture&)            = delete;
  Texture& operator=(const Texture&) = delete;

  // Load image from disk. Prints a warning and creates a 1×1 grey fallback
  // if the file is not found, so rendering continues without crashing.
  void loadFromFile(const std::string& path);

  // Bind to a texture unit (default unit 0).
  void bind(int unit = 0) const;

  GLuint id()       const { return m_id; }
  bool   isLoaded() const { return m_id != 0; }

private:
  GLuint m_id{0};
};

#endif
