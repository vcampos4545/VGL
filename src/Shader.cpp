#include <vaughngl/Shader.h>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <glm/gtc/type_ptr.hpp>

Shader::Shader(const char* vertexPath, const char* fragmentPath) {
  load(vertexPath, fragmentPath);
}

Shader::~Shader() {
  if (m_id) glDeleteProgram(m_id);
}

Shader::Shader(Shader&& other) noexcept : m_id(other.m_id) {
  other.m_id = 0;
}

Shader& Shader::operator=(Shader&& other) noexcept {
  if (this != &other) {
    if (m_id) glDeleteProgram(m_id);
    m_id = other.m_id;
    other.m_id = 0;
  }
  return *this;
}

void Shader::load(const char* vertexPath, const char* fragmentPath) {
  std::string vertCode = loadSource(vertexPath);
  std::string fragCode = loadSource(fragmentPath);
  loadFromSource(vertCode.c_str(), fragCode.c_str());
}

void Shader::loadFromSource(const char* vertexSource, const char* fragmentSource) {
  if (m_id) glDeleteProgram(m_id);

  GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex, 1, &vertexSource, NULL);
  glCompileShader(vertex);
  checkErrors(vertex, "VERTEX");

  GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment, 1, &fragmentSource, NULL);
  glCompileShader(fragment);
  checkErrors(fragment, "FRAGMENT");

  m_id = glCreateProgram();
  glAttachShader(m_id, vertex);
  glAttachShader(m_id, fragment);
  glLinkProgram(m_id);
  checkErrors(m_id, "PROGRAM");

  glDeleteShader(vertex);
  glDeleteShader(fragment);
}

void Shader::use() const {
  glUseProgram(m_id);
}

void Shader::setBool(const std::string& name, bool value) const {
  glUniform1i(glGetUniformLocation(m_id, name.c_str()), (int)value);
}

void Shader::setFloat(const std::string& name, float value) const {
  glUniform1f(glGetUniformLocation(m_id, name.c_str()), value);
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const {
  glUniform3fv(glGetUniformLocation(m_id, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::setMat4(const std::string& name, const glm::mat4& mat) const {
  glUniformMatrix4fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

std::string Shader::loadSource(const char* path) {
  std::ifstream file;
  file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  try {
    file.open(path);
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
  } catch (std::ifstream::failure& e) {
    printf("\033[31mShader file not found: %s\033[0m\n", path);
    return "";
  }
}

void Shader::checkErrors(GLuint shader, const std::string& type) {
  GLint success;
  GLchar log[1024];

  if (type != "PROGRAM") {
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(shader, 1024, NULL, log);
      printf("\033[31mShader compile error (%s): %s\033[0m\n", type.c_str(), log);
    }
  } else {
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if (!success) {
      glGetProgramInfoLog(shader, 1024, NULL, log);
      printf("\033[31mShader link error: %s\033[0m\n", log);
    }
  }
}
