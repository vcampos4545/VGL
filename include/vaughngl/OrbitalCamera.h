#pragma once

#include "Camera.h"
#include "GUI.h"
#include <glm/glm.hpp>

// Arcball-style orbital camera controller.
// Left-drag  : rotate (yaw/pitch)
// Right-drag : pan target point
// Scroll     : zoom
class OrbitalCamera
{
public:
  OrbitalCamera(float distance,
                float yaw,
                float pitch,
                glm::vec3 target);

  // Chainable configuration
  OrbitalCamera &setMaxDistance(float d);
  OrbitalCamera &setMinDistance(float d);
  OrbitalCamera &setZoomSensitivity(float s);
  OrbitalCamera &setPanSensitivity(float s);

  // Call once per frame before applyToCamera
  void handleInput(GUI &gui,
                   glm::vec2 mouseDelta,
                   glm::vec2 scrollDelta);

  // Writes position and target into the given Camera
  void applyToCamera(Camera &camera) const;

private:
  float distance;
  float yaw;   // degrees
  float pitch; // degrees, clamped to [-89, 89]
  glm::vec3 target;

  float maxDistance    = 1e15f;
  float minDistance    = 0.1f;
  float panSensitivity = 0.1f;
  float zoomSensitivity = 1.0f;
};
