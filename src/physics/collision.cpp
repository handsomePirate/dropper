#include "collision.h"

namespace Physics {

bool AABB(const glm::vec3 &lineStart, const glm::vec3 &lineEnd,
          const glm::vec3 &aa, const glm::vec3 &bb, glm::vec3 &result) {

  float t[6];
  glm::vec3 r[6];
  auto dir = lineEnd - lineStart;

  if (dir.y != 0) {
    // top
    t[0] = (bb.y - lineStart.y) / dir.y;
    r[0] = lineStart + t[0] * dir;
    if (t[0] < 0 || r[0].x < aa.x || r[0].x > bb.x || r[0].z < aa.z ||
        r[0].z > bb.z)
      t[0] = 2;
    // bottom
    t[1] = (aa.y - lineStart.y) / dir.y;
    r[1] = lineStart + t[1] * dir;
    if (t[1] < 0 || r[1].x < aa.x || r[1].x > bb.x || r[1].z < aa.z ||
        r[1].z > bb.z)
      t[1] = 2;
  } else {
    t[0] = 2;
    t[1] = 2;
  }

  if (dir.z != 0) {
    // front
    t[2] = (bb.z - lineStart.z) / dir.z;
    r[2] = lineStart + t[2] * dir;
    if (t[2] < 0 || r[2].x < aa.x || r[2].x > bb.x || r[2].z < aa.z ||
        r[2].z > bb.z)
      t[2] = 2;
    // back
    t[3] = (aa.z - lineStart.z) / dir.z;
    r[3] = lineStart + t[3] * dir;
    if (t[3] < 0 || r[3].x < aa.x || r[3].x > bb.x || r[3].z < aa.z ||
        r[3].z > bb.z)
      t[3] = 2;
  } else {
    t[2] = 2;
    t[3] = 2;
  }

  if (dir.x != 0) {
    // left
    t[4] = (bb.x - lineStart.x) / dir.x;
    r[4] = lineStart + t[4] * dir;
    if (t[4] < 0 || r[4].x < aa.x || r[4].x > bb.x || r[4].z < aa.z ||
        r[4].z > bb.z)
      t[4] = 2;
    // right
    t[5] = (aa.x - lineStart.x) / dir.x;
    r[5] = lineStart + t[5] * dir;
    if (t[5] < 0 || r[5].x < aa.x || r[5].x > bb.x || r[5].z < aa.z ||
        r[5].z > bb.z)
      t[5] = 2;
  } else {
    t[4] = 2;
    t[5] = 2;
  }

  uint32_t minIndex = UINT32_MAX;
  float min = 2;
  for (int i = 0; i < 6; ++i) {
    if (t[i] < min) {
      minIndex = i;
      min = t[i];
    }
  }

  if (minIndex != UINT32_MAX) {
    result = r[minIndex];
    return true;
  }
  return false;
}

} // namespace Physics
