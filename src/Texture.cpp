#include <vgl/Texture.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <cstdio>

Texture::~Texture() {
  if (m_id) glDeleteTextures(1, &m_id);
}

Texture::Texture(Texture&& other) noexcept : m_id(other.m_id) {
  other.m_id = 0;
}

Texture& Texture::operator=(Texture&& other) noexcept {
  if (this != &other) {
    if (m_id) glDeleteTextures(1, &m_id);
    m_id = other.m_id;
    other.m_id = 0;
  }
  return *this;
}

void Texture::loadFromFile(const std::string& path) {
  if (m_id) { glDeleteTextures(1, &m_id); m_id = 0; }

  stbi_set_flip_vertically_on_load(true);
  int w, h, ch;
  unsigned char* data = stbi_load(path.c_str(), &w, &h, &ch, 0);

  glGenTextures(1, &m_id);
  glBindTexture(GL_TEXTURE_2D, m_id);

  if (!data) {
    std::printf("[Texture] Could not load: %s\n", path.c_str());
    unsigned char grey[3] = {180, 180, 180};
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, grey);
    return;
  }

  GLenum fmt = (ch == 4) ? GL_RGBA : GL_RGB;
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, fmt, w, h, 0, fmt, GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);
  stbi_image_free(data);
}

void Texture::bind(int unit) const {
  glActiveTexture(GL_TEXTURE0 + unit);
  glBindTexture(GL_TEXTURE_2D, m_id);
}
