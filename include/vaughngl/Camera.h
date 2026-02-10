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

  // Chainable setters
  Camera& setPosition(glm::vec3 pos) { position = pos; return *this; }
  Camera& setTarget(glm::vec3 tgt) { target = tgt; return *this; }
  Camera& setUp(glm::vec3 upVec) { up = upVec; return *this; }
  Camera& setFOV(float degrees) { fov = degrees; return *this; }
  Camera& setNearPlane(float near) { nearPlane = near; return *this; }
  Camera& setFarPlane(float far) { farPlane = far; return *this; }
  Camera& setClipPlanes(float near, float far) { nearPlane = near; farPlane = far; return *this; }

  // Convenience method to set position and target together
  Camera& lookAt(glm::vec3 pos, glm::vec3 tgt) {
    position = pos;
    target = tgt;
    return *this;
  }

  // Computed direction vectors
  glm::vec3 getDirection() const { return glm::normalize(target - position); }
  glm::vec3 getRight() const { return glm::normalize(glm::cross(getDirection(), up)); }
  glm::vec3 getUpVector() const { return glm::normalize(glm::cross(getRight(), getDirection())); }

  // Distance from camera to target
  float getDistance() const { return glm::length(target - position); }

  // Move camera while maintaining distance to target
  Camera& setDistance(float dist) {
    glm::vec3 dir = getDirection();
    position = target - dir * dist;
    return *this;
  }

  // Matrices
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
