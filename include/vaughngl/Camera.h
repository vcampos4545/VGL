#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
  glm::vec3 position{0, 0, 5};
  glm::vec3 target{0, 0, 0};
  glm::vec3 up{0, 1, 0};

  float fov = 45.0f;
  float nearPlane = 0.1f;
  float farPlane = 100.0f;

  glm::mat4 getViewMatrix() const {
    return glm::lookAt(position, target, up);
  }

  glm::mat4 getProjectionMatrix(float aspect) const {
    return glm::perspective(glm::radians(fov), aspect, nearPlane, farPlane);
  }

  // For 2D/orthographic rendering
  glm::mat4 getOrthoMatrix(float width, float height) const {
    return glm::ortho(0.0f, width, 0.0f, height, -1.0f, 1.0f);
  }
};

#endif
