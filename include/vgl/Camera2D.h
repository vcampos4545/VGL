#ifndef VGL_CAMERA2D_H
#define VGL_CAMERA2D_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

// World-space orthographic camera for 2D rendering (Y-up).
//
// The camera is defined by a world-space center point and a vertical
// "view height" (world units spanned top-to-bottom); the horizontal span
// is derived from the viewport aspect ratio, so panning/zooming behaves
// consistently regardless of window shape.
class Camera2D {
public:
  glm::vec2 center{0.0f, 0.0f};
  float viewHeight = 10.0f;

  Camera2D &setCenter(glm::vec2 c) { center = c; return *this; }
  Camera2D &setViewHeight(float h) { viewHeight = std::max(h, 1e-5f); return *this; }
  Camera2D &pan(glm::vec2 worldDelta) { center += worldDelta; return *this; }
  Camera2D &zoom(float factor) { viewHeight = std::max(viewHeight * factor, 1e-5f); return *this; }

  glm::vec2 halfExtents(float aspect) const {
    float halfH = viewHeight * 0.5f;
    return {halfH * aspect, halfH};
  }

  glm::mat4 getViewMatrix() const {
    return glm::lookAt(glm::vec3(center, 1.0f), glm::vec3(center, 0.0f), glm::vec3(0, 1, 0));
  }

  glm::mat4 getProjectionMatrix(float aspect) const {
    glm::vec2 half = halfExtents(aspect);
    return glm::ortho(-half.x, half.x, -half.y, half.y, -1.0f, 1.0f);
  }

  glm::mat4 getViewProjection(float aspect) const {
    return getProjectionMatrix(aspect) * getViewMatrix();
  }

  // screenPx: pixel coordinates with origin top-left (GLFW convention).
  glm::vec2 screenToWorld(glm::vec2 screenPx, glm::vec2 viewportSize) const {
    float aspect = viewportSize.x / viewportSize.y;
    glm::vec2 half = halfExtents(aspect);
    float ndcX = (screenPx.x / viewportSize.x) * 2.0f - 1.0f;
    float ndcY = 1.0f - (screenPx.y / viewportSize.y) * 2.0f;
    return center + glm::vec2(ndcX * half.x, ndcY * half.y);
  }

  glm::vec2 worldToScreen(glm::vec2 world, glm::vec2 viewportSize) const {
    float aspect = viewportSize.x / viewportSize.y;
    glm::vec2 half = halfExtents(aspect);
    glm::vec2 rel = world - center;
    float ndcX = rel.x / half.x;
    float ndcY = rel.y / half.y;
    float sx = (ndcX * 0.5f + 0.5f) * viewportSize.x;
    float sy = (1.0f - (ndcY * 0.5f + 0.5f)) * viewportSize.y;
    return {sx, sy};
  }

  // World-space size of one screen pixel at the current zoom level (uniform, since
  // the camera never rotates or skews — useful for hit-testing and snap tolerances).
  float worldUnitsPerPixel(glm::vec2 viewportSize) const {
    return viewHeight / viewportSize.y;
  }
};

#endif
