#include "slab-event.h"
#include "game/resources.h"
#include "graphics/graphics.h"
#include "physics/collision.h"

#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

namespace Levels {

namespace {

bool stringsEqual(const char *base, const char *copy, uint32_t length) {
  for (int i = 0; i < length; ++i) {
    if (base[i] != copy[i]) {
      return false;
    }
  }
  return true;
}

} // namespace

SlabEvent::SlabEvent(Graphics::Context graphics_, Graphics::Model model_,
                     Graphics::Material material_, const char *line)
    : graphics(graphics_), material(material_), model(model_), spawned(false) {
  Graphics::allocateInstanceBatches(
      graphics, 1, (Graphics::InstanceBatchT **)&instanceBatch);
  int start = 0, curr = 0;
  int stage = 0;
  float height;
  float x, y, sx, sy;
  isValid = false;
  while (true) {
    if (line[curr] == ' ' or line[curr] == '\0') {
      if (stage == 0) {
        if (stringsEqual("e0", line + start, curr - start)) {
        } else {
          std::cout << "Incorrect event label, expected 'e0'\n";
        }
      } else if (stage == 1) {
        // TODO: try-catch
        height = std::stof(std::string(line + start, curr - start));
      } else if (stage == 2) {
        x = std::stof(std::string(line + start, curr - start));
      } else if (stage == 3) {
        y = std::stof(std::string(line + start, curr - start));
      } else if (stage == 4) {
        sx = std::stof(std::string(line + start, curr - start));
      } else if (stage == 5) {
        sy = std::stof(std::string(line + start, curr - start));
      } else {
        if (line[curr] != '\0') {
          std::cout << "Event entry too long\n";
          return;
        }
        break;
      }

      start = curr + 1;
      ++stage;
    }
    ++curr;
  }

  position = {x, height, y};
  size = {sx, 1, sy};

  isValid = true;
}

SlabEvent::~SlabEvent() {
  if (not isValid)
    return;

  if (spawned) {
    Graphics::destroyInstanceBatch(graphics, instanceBatch);
  }
  Graphics::freeInstanceBatches(graphics, instanceBatch);
}

bool SlabEvent::getShouldSpawn(float height_) {
  if (not isValid)
    return false;
  return height_ - position.y < Game::spawnDistance;
}

void SlabEvent::spawn(Graphics::Context graphics) {
  if (not isValid)
    return;
  spawned = true;
  auto transform = glm::scale(glm::translate(glm::mat4(1), position), size);
  instance = transform;
  Graphics::createInstanceBatch(graphics, instanceBatch, model, &instance, 1);
}

void SlabEvent::draw(Graphics::Context graphics) {
  if (isValid and spawned) {
    Graphics::drawInstanceBatch(graphics, instanceBatch, material);
  }
}

bool SlabEvent::getCouldCollide(float start, float next) {
  if (not isValid)
    return false;
  float shift = 0.5f * (1 + Game::playerSize);
  float high = position.y + shift;
  float low = position.y - shift;
  return not(start > high and next > high) and not(start < low and next < low);
}
bool SlabEvent::collide(const glm::vec3 &start, const glm::vec3 &next,
                        glm::vec3 &collision) {
  if (not isValid)
    return false;
  auto aa = position - 0.5f * size - glm::vec3(Game::playerSize) * 0.5f;
  auto bb = position + 0.5f * size + glm::vec3(Game::playerSize) * 0.5f;
  return Physics::AABB(start, next, aa, bb, collision);
}
bool SlabEvent::isWinCollision(const glm::vec3 &collision) { return false; }

SlabCombinationEvent::SlabCombinationEvent(Graphics::Context graphics_,
                                           Graphics::Model model_,
                                           Graphics::Material material_,
                                           const char *line)
    : graphics(graphics_), material(material_), model(model_), spawned(false) {
  int start = 0, curr = 0;
  int stage = 0;
  const char *instanceGrid_;
  isValid = false;
  while (true) {
    if (line[curr] == ' ' or line[curr] == '\0') {
      if (stage == 0) {
        if (stringsEqual("e1", line + start, curr - start)) {
        } else {
          std::cout << "Incorrect event label, expected 'e1'\n";
        }
      } else if (stage == 1) {
        // TODO: try-catch
        height = std::stof(std::string(line + start, curr - start));
      } else if (stage == 2) {
        auto test = sqrtf(float(curr - start));
        if (test != truncf(test)) {
          std::cout << "The number of t/f must be a second power for tiling "
                       "purposes\n";
          return;
        }
        edge = int(test);
        if (line[curr] != '\0') {
          std::cout << "Event string too long\n";
          return;
        }
        instanceGrid_ = line + start;
        break;
      }

      start = curr + 1;
      ++stage;
    }
    ++curr;
  }

  uint32_t edge2 = edge * edge;

  count = 0;
  for (int i = 0; i < edge2; ++i) {
    count += instanceGrid_[i] == 't' ? 1 : 0;
  }

  instanceGrid.resize(edge2);
  for (int i = 0; i < edge2; ++i) {
    instanceGrid[i] = instanceGrid_[i] == 't';
  }

  instanceMemory.resize(count);

  Graphics::allocateInstanceBatches(
      graphics, 1, (Graphics::InstanceBatchT **)&instanceBatch);

  isValid = true;
}

SlabCombinationEvent::~SlabCombinationEvent() {
  if (not isValid)
    return;

  if (spawned) {
    Graphics::destroyInstanceBatch(graphics, instanceBatch);
  }
  Graphics::freeInstanceBatches(graphics, instanceBatch);
}

bool SlabCombinationEvent::getShouldSpawn(float height_) {
  if (not isValid)
    return false;
  return height_ - height < Game::spawnDistance;
}

void SlabCombinationEvent::spawn(Graphics::Context graphics) {
  if (not isValid)
    return;

  spawned = true;
  uint32_t k = 0;
  float sizeDiv = Game::tubeDiameter / edge;
  glm::vec3 size{sizeDiv, 1, sizeDiv};
  auto scale = glm::scale(glm::mat4(1), size);
  for (int i = 0; i < edge; ++i) {
    for (int j = 0; j < edge; ++j) {
      auto instanceExists = instanceGrid[i * edge + j];
      if (instanceExists) {
        float x = -Game::tubeDiameter * 0.5f + size.x * (0.5f + i);
        float y = -Game::tubeDiameter * 0.5f + size.z * (0.5f + j);
        glm::vec3 position{x, height, y};
        auto translate = glm::translate(glm::mat4(1), position);
        instanceMemory[k++] = translate * scale;
      }
    }
  }

  Graphics::createInstanceBatch(graphics, instanceBatch, model,
                                instanceMemory.data(), count);
}

void SlabCombinationEvent::draw(Graphics::Context graphics) {
  if (isValid and spawned) {
    Graphics::drawInstanceBatch(graphics, instanceBatch, material);
  }
}

bool SlabCombinationEvent::getCouldCollide(float start, float next) {
  if (not isValid)
    return false;
  float shift = 0.5f * (1 + Game::playerSize);
  float high = height + shift;
  float low = height - shift;
  return not(start > high and next > high) and not(start < low and next < low);
}

bool SlabCombinationEvent::collide(const glm::vec3 &start,
                                   const glm::vec3 &next,
                                   glm::vec3 &collision) {
  if (not isValid)
    return false;
  float sizeDiv = Game::tubeDiameter / edge;
  glm::vec3 size{sizeDiv, 1, sizeDiv};
  for (int i = 0; i < edge; ++i) {
    for (int j = 0; j < edge; ++j) {
      auto instanceExists = instanceGrid[i * edge + j];
      if (instanceExists) {
        float x = -Game::tubeDiameter * 0.5f + size.x * (0.5f + i);
        float y = -Game::tubeDiameter * 0.5f + size.z * (0.5f + j);
        glm::vec3 position{x, height, y};
        auto aa = position - 0.5f * size - glm::vec3(Game::playerSize) * 0.5f;
        auto bb = position + 0.5f * size + glm::vec3(Game::playerSize) * 0.5f;
        if (Physics::AABB(start, next, aa, bb, collision)) {
          Physics::AABB(start, next, aa, bb, collision);
          return true;
        }
      }
    }
  }
  return false;
}

bool SlabCombinationEvent::isWinCollision(const glm::vec3 &collision) {
  return false;
}

} // namespace Levels
