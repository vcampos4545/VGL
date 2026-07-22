#include <vgl/DataTexture.h>

DataTexture::~DataTexture() {
  if (m_id) glDeleteTextures(1, &m_id);
}

DataTexture::DataTexture(DataTexture &&other) noexcept
    : m_id(other.m_id), m_width(other.m_width), m_height(other.m_height),
      m_internalFormat(other.m_internalFormat) {
  other.m_id = 0;
}

DataTexture &DataTexture::operator=(DataTexture &&other) noexcept {
  if (this != &other) {
    if (m_id) glDeleteTextures(1, &m_id);
    m_id = other.m_id;
    m_width = other.m_width;
    m_height = other.m_height;
    m_internalFormat = other.m_internalFormat;
    other.m_id = 0;
  }
  return *this;
}

void DataTexture::ensureAllocated(int w, int h, GLenum internalFormat, GLenum format) {
  bool needsAlloc = (m_id == 0) || (m_width != w) || (m_height != h) || (m_internalFormat != internalFormat);

  if (m_id == 0) {
    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_2D, m_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  } else {
    glBindTexture(GL_TEXTURE_2D, m_id);
  }

  if (needsAlloc) {
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, w, h, 0, format, GL_FLOAT, nullptr);
    m_width = w;
    m_height = h;
    m_internalFormat = internalFormat;
  }
}

void DataTexture::uploadR32F(const float *data, int w, int h) {
  ensureAllocated(w, h, GL_R32F, GL_RED);
  glBindTexture(GL_TEXTURE_2D, m_id);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, GL_RED, GL_FLOAT, data);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
}

void DataTexture::uploadRG32F(const float *data, int w, int h) {
  ensureAllocated(w, h, GL_RG32F, GL_RG);
  glBindTexture(GL_TEXTURE_2D, m_id);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, GL_RG, GL_FLOAT, data);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
}

void DataTexture::bind(int unit) const {
  glActiveTexture(GL_TEXTURE0 + unit);
  glBindTexture(GL_TEXTURE_2D, m_id);
}
