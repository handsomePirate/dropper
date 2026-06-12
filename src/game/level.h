#pragma once
#include "game/resources.h"

#include <glm/glm.hpp>

#include <memory>
#include <stdint.h>

namespace Graphics {
using Context = struct ContextT *;
} // namespace Graphics

namespace Game {

static constexpr float spawnDistance = tubeLength + 5.f;
static constexpr float playerSize = 0.2f;

class Event {
public:
  virtual ~Event() {}

  virtual bool getShouldSpawn(float height) = 0;
  virtual void spawn(Graphics::Context graphics) = 0;
  virtual void draw(Graphics::Context graphics) = 0;

  virtual bool getCouldCollide(float start, float next) = 0;
  virtual bool collide(const glm::vec3 &start, const glm::vec3 &next,
                       glm::vec3 &collision) = 0;
  virtual bool isWinCollision(const glm::vec3 &collision) = 0;
};

struct Level {
  std::vector<std::unique_ptr<Event>> eventQueue;
  uint32_t spawnIndex;
  uint32_t collideIndex;
};

void loadLevel(Graphics::Context graphics, Level *level);
void updateLevel(Graphics::Context graphics, Level *level,
                 float timeDeltaSeconds);

} // namespace Game
