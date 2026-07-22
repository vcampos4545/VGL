#ifndef VGL_TRIANGULATION_H
#define VGL_TRIANGULATION_H

#include <glm/glm.hpp>
#include <vector>
#include <cstdint>

// Simple polygon triangulation utilities for filling arbitrary (possibly
// concave, non-self-intersecting) 2D polygons.
namespace Triangulation {

// Ear-clipping triangulation. Accepts a simple polygon in either winding
// order; returns triangle indices into `points` (3 indices per triangle).
// Returns an empty vector if the polygon is degenerate (< 3 points).
std::vector<uint32_t> earClip(const std::vector<glm::vec2> &points);

} // namespace Triangulation

#endif
