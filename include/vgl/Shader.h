#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>

class Shader {
public:
  Shader() = default;
  Shader(const char* vertexPath, const char* fragmentPath);
  ~Shader();

  Shader(Shader&& other) noexcept;
  Shader& operator=(Shader&& other) noexcept;
  Shader(const Shader&) = delete;
  Shader& operator=(const Shader&) = delete;

  void load(const char* vertexPath, const char* fragmentPath);
  void loadFromSource(const char* vertexSource, const char* fragmentSource);
  void use() const;

  void setBool(const std::string& name, bool value) const;
  void setInt(const std::string& name, int value) const;
  void setFloat(const std::string& name, float value) const;
  void setVec3(const std::string& name, const glm::vec3& value) const;
  void setMat4(const std::string& name, const glm::mat4& mat) const;

  GLuint getID() const { return m_id; }

private:
  GLuint m_id = 0;
  mutable std::unordered_map<std::string, GLint> m_uniformCache;

  std::string loadSource(const char* path);
  void checkErrors(GLuint shader, const std::string& type);
  GLint uniformLocation(const std::string& name) const;
};

#endif
