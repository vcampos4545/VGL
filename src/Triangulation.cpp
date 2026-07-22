#include <vgl/Triangulation.h>
#include <numeric>

namespace {

float signedArea(const std::vector<glm::vec2> &pts) {
  float area = 0.0f;
  for (size_t i = 0; i < pts.size(); ++i) {
    const glm::vec2 &a = pts[i];
    const glm::vec2 &b = pts[(i + 1) % pts.size()];
    area += a.x * b.y - b.x * a.y;
  }
  return area * 0.5f;
}

bool pointInTriangle(glm::vec2 p, glm::vec2 a, glm::vec2 b, glm::vec2 c) {
  float d1 = (p.x - b.x) * (a.y - b.y) - (a.x - b.x) * (p.y - b.y);
  float d2 = (p.x - c.x) * (b.y - c.y) - (b.x - c.x) * (p.y - c.y);
  float d3 = (p.x - a.x) * (c.y - a.y) - (c.x - a.x) * (p.y - a.y);
  bool hasNeg = (d1 < 0) || (d2 < 0) || (d3 < 0);
  bool hasPos = (d1 > 0) || (d2 > 0) || (d3 > 0);
  return !(hasNeg && hasPos);
}

bool isConvex(glm::vec2 a, glm::vec2 b, glm::vec2 c) {
  float cross = (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
  return cross > 0.0f; // CCW winding assumed
}

} // namespace

namespace Triangulation {

std::vector<uint32_t> earClip(const std::vector<glm::vec2> &pointsIn) {
  std::vector<uint32_t> result;
  if (pointsIn.size() < 3) return result;

  std::vector<glm::vec2> pts = pointsIn;
  std::vector<uint32_t> indices(pts.size());
  std::iota(indices.begin(), indices.end(), 0u);

  // Ear clipping requires CCW winding; flip index order if the polygon is CW.
  if (signedArea(pts) < 0.0f) {
    std::reverse(indices.begin(), indices.end());
  }

  size_t guard = 0;
  const size_t maxIterations = indices.size() * indices.size() + 8;

  while (indices.size() > 3 && guard < maxIterations) {
    bool clipped = false;
    size_t n = indices.size();

    for (size_t i = 0; i < n; ++i) {
      uint32_t iPrev = indices[(i + n - 1) % n];
      uint32_t iCur = indices[i];
      uint32_t iNext = indices[(i + 1) % n];

      glm::vec2 a = pts[iPrev], b = pts[iCur], c = pts[iNext];
      if (!isConvex(a, b, c)) continue;

      bool anyInside = false;
      for (size_t j = 0; j < n; ++j) {
        uint32_t idx = indices[j];
        if (idx == iPrev || idx == iCur || idx == iNext) continue;
        if (pointInTriangle(pts[idx], a, b, c)) {
          anyInside = true;
          break;
        }
      }
      if (anyInside) continue;

      result.push_back(iPrev);
      result.push_back(iCur);
      result.push_back(iNext);
      indices.erase(indices.begin() + i);
      clipped = true;
      break;
    }

    if (!clipped) break; // degenerate/self-intersecting input; stop gracefully
    ++guard;
  }

  if (indices.size() == 3) {
    result.push_back(indices[0]);
    result.push_back(indices[1]);
    result.push_back(indices[2]);
  }

  return result;
}

} // namespace Triangulation
