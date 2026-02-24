#include <vaughngl/OrbitalCamera.h>
#include <cmath>

OrbitalCamera::OrbitalCamera(float dist, float y, float p, glm::vec3 tgt)
    : distance(dist), yaw(y), pitch(p), target(tgt)
{
}

OrbitalCamera &OrbitalCamera::setMaxDistance(float d)
{
  maxDistance = d;
  return *this;
}

OrbitalCamera &OrbitalCamera::setMinDistance(float d)
{
  minDistance = d;
  return *this;
}

OrbitalCamera &OrbitalCamera::setZoomSensitivity(float s)
{
  zoomSensitivity = s;
  return *this;
}

OrbitalCamera &OrbitalCamera::setPanSensitivity(float s)
{
  panSensitivity = s;
  return *this;
}

void OrbitalCamera::handleInput(GUI &gui, glm::vec2 mouseDelta, glm::vec2 scrollDelta)
{
  // Rotate with left mouse drag
  if (gui.isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
  {
    yaw -= mouseDelta.x * panSensitivity;
    pitch += mouseDelta.y * panSensitivity;
    pitch = glm::clamp(pitch, -89.0f, 89.0f);
  }

  // Pan with right mouse drag
  if (gui.isMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT))
  {
    glm::vec3 forward = glm::normalize(target - gui.camera.position);
    glm::vec3 right   = glm::normalize(glm::cross(forward, gui.camera.up));
    target -= right         * mouseDelta.x * panSensitivity;
    target += gui.camera.up * mouseDelta.y * panSensitivity;
  }

  // Zoom with scroll wheel
  distance -= scrollDelta.y * zoomSensitivity;
  distance = glm::clamp(distance, minDistance, maxDistance);
}

void OrbitalCamera::applyToCamera(Camera &camera) const
{
  float yawRad   = glm::radians(yaw);
  float pitchRad = glm::radians(pitch);

  camera.position = target + glm::vec3(
                                 distance * std::cos(pitchRad) * std::cos(yawRad),
                                 distance * std::cos(pitchRad) * std::sin(yawRad),
                                 distance * std::sin(pitchRad));
  camera.target = target;
}
