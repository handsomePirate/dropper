#pragma once
#include "game/level.h"
#include "graphics/graphics.h"

#include <glm/glm.hpp>

#include <vector>

namespace Levels {

class SlabEvent : public Game::Event {
public:
  SlabEvent(Graphics::Context graphics, Graphics::Model model,
            Graphics::Material material, const char *line);
  ~SlabEvent();

  bool getShouldSpawn(float height) override;
  void spawn(Graphics::Context graphics) override;
  void draw(Graphics::Context graphics) override;

  bool getCouldCollide(float start, float next) override;
  bool collide(const glm::vec3 &start, const glm::vec3 &next,
               glm::vec3 &collision) override;
  bool isWinCollision(const glm::vec3 &collision) override;

private:
  glm::mat4 instance;
  Graphics::Context graphics;
  Graphics::Material material;
  Graphics::Model model;
  Graphics::InstanceBatch instanceBatch;

  glm::vec3 position;
  glm::vec3 size;

  bool spawned;
  bool isValid;
};

class SlabCombinationEvent : public Game::Event {
public:
  SlabCombinationEvent(Graphics::Context graphics, Graphics::Model model,
                       Graphics::Material material, const char *line);
  ~SlabCombinationEvent();

  bool getShouldSpawn(float height) override;
  void spawn(Graphics::Context graphics) override;
  void draw(Graphics::Context graphics) override;

  bool getCouldCollide(float start, float next) override;
  bool collide(const glm::vec3 &start, const glm::vec3 &next,
               glm::vec3 &collision) override;
  bool isWinCollision(const glm::vec3 &collision) override;

private:
  Graphics::Context graphics;
  Graphics::Material material;

  std::vector<bool> instanceGrid;
  std::vector<glm::mat4> instanceMemory;
  Graphics::InstanceBatch instanceBatch;
  Graphics::Model model;

  uint32_t count;
  float height;
  uint32_t edge;

  bool spawned;
  bool isValid;
};

} // namespace Levels
