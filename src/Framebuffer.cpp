#include <vgl/Framebuffer.h>
#include <cstdio>

Framebuffer::~Framebuffer() { destroy(); }

Framebuffer::Framebuffer(Framebuffer &&other) noexcept
    : m_fbo(other.m_fbo), m_colorTex(other.m_colorTex), m_depthRbo(other.m_depthRbo),
      m_width(other.m_width), m_height(other.m_height), m_float(other.m_float) {
  other.m_fbo = other.m_colorTex = other.m_depthRbo = 0;
}

Framebuffer &Framebuffer::operator=(Framebuffer &&other) noexcept {
  if (this != &other) {
    destroy();
    m_fbo = other.m_fbo;
    m_colorTex = other.m_colorTex;
    m_depthRbo = other.m_depthRbo;
    m_width = other.m_width;
    m_height = other.m_height;
    m_float = other.m_float;
    other.m_fbo = other.m_colorTex = other.m_depthRbo = 0;
  }
  return *this;
}

void Framebuffer::destroy() {
  if (m_depthRbo) glDeleteRenderbuffers(1, &m_depthRbo);
  if (m_colorTex) glDeleteTextures(1, &m_colorTex);
  if (m_fbo) glDeleteFramebuffers(1, &m_fbo);
  m_fbo = m_colorTex = m_depthRbo = 0;
}

void Framebuffer::create(int width, int height, bool floatingPoint) {
  destroy();
  m_width = width;
  m_height = height;
  m_float = floatingPoint;

  glGenFramebuffers(1, &m_fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

  glGenTextures(1, &m_colorTex);
  glBindTexture(GL_TEXTURE_2D, m_colorTex);
  GLenum internalFormat = m_float ? GL_RGBA16F : GL_RGBA8;
  GLenum type = m_float ? GL_FLOAT : GL_UNSIGNED_BYTE;
  glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, GL_RGBA, type, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorTex, 0);

  glGenRenderbuffers(1, &m_depthRbo);
  glBindRenderbuffer(GL_RENDERBUFFER, m_depthRbo);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_depthRbo);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    std::fprintf(stderr, "[Framebuffer] Incomplete framebuffer (%dx%d)\n", width, height);
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::resize(int width, int height) {
  if (width == m_width && height == m_height) return;
  if (width <= 0 || height <= 0) return;
  create(width, height, m_float);
}

void Framebuffer::bind() const {
  glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
  glViewport(0, 0, m_width, m_height);
}

void Framebuffer::unbind() {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
