#pragma once
#include <glm/glm.hpp>

namespace Physics {

bool AABB(const glm::vec3 &lineStart, const glm::vec3 &lineEnd,
          const glm::vec3 &aa, const glm::vec3 &bb, glm::vec3 &result);

}
