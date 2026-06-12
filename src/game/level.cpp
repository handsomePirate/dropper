#include "level.h"
#include "controls/controls.h"
#include "game/resources.h"
#include "graphics/graphics.h"
#include "levels/file.h"

#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>
#include <assert.h>

namespace Game {

namespace {

Event *spawnEvent = nullptr;
Event *collideEvent = nullptr;
bool lastCouldCollide;
float proximityMultiplierTarget;
float gaspTrauma;
glm::vec3 tintTarget;
bool gaspPlaying;

Event *popSpawn(Level *level) {
  if (level->spawnIndex >= level->eventQueue.size())
    return nullptr;

  return level->eventQueue[level->spawnIndex++].get();
}

Event *popCollide(Level *level) {
  if (level->collideIndex >= level->spawnIndex)
    return nullptr;

  return level->eventQueue[level->collideIndex++].get();
}

} // namespace

void loadLevel(Graphics::Context graphics, Level *level) {
  spawnEvent = popSpawn(level);
  collideEvent = popCollide(level);
  lastCouldCollide = false;
  proximityMultiplierTarget = 1.f;
  gaspTrauma = 0.f;
  tintTarget = glm::vec3{1};
}

void updateLevel(Graphics::Context graphics, Level *level,
                 float timeDeltaSeconds) {
  if (spawnEvent && spawnEvent->getShouldSpawn(cameraPosition.y)) {
    spawnEvent->spawn(graphics);
    spawnEvent = popSpawn(level);
  }

  if (collideEvent &&
      collideEvent->getCouldCollide(lastCameraPosition.y, cameraPosition.y)) {
    glm::vec3 collision;
    if (collideEvent->collide(lastCameraPosition, cameraPosition, collision)) {
      Game::levelEnded = true;
      Game::levelWon = collideEvent->isWinCollision(collision);
      proximityMultiplierTarget = 0.f;
      cameraPosition = lastCameraPosition;

      if (Game::levelWon) {
        tintTarget = glm::vec3(0, 1, 0);
        Audio::playSound(swoosh);
      } else {
        tintTarget = glm::vec3(1, 0, 0);
        Audio::playSound(crash);
      }
      collideEvent = nullptr;
    }
    lastCouldCollide = true;
  } else if (lastCouldCollide && not Game::levelEnded) {
    collideEvent = popCollide(level);
    lastCouldCollide = false;
  }

  gaspTrauma = std::max(gaspTrauma - timeDeltaSeconds * 1.f, 0.f);

  glm::vec3 collision;
  const float predictionSize = cameraSpeed / 3.f;
  auto cameraPrediction = glm::normalize(cameraVelocity) * predictionSize;
  auto predictedCameraPosition = cameraPrediction + cameraPosition;
  if (proximityMultiplierTarget > 0.f) {
    proximityMultiplierTarget = 1.f;
    if (collideEvent && collideEvent->getCouldCollide(
                            cameraPosition.y, predictedCameraPosition.y)) {
      if (collideEvent->collide(cameraPosition, predictedCameraPosition,
                                collision)) {
        proximityMultiplierTarget = std::clamp(
            glm::distance(cameraPosition, collision) / predictionSize, 0.f,
            1.f);
        gaspTrauma = 1.f - proximityMultiplierTarget;
      }
    }
  }

  if (gaspTrauma >= 0.6f) {
    if (not gaspPlaying) {
      Audio::playSound(gasp);
      gaspPlaying = true;
    }
  }
  if (gaspTrauma <= 0.1f) {
    gaspPlaying = false;
  }

  const float targetWeight = std::min(3.0f * timeDeltaSeconds, 1.f);
  Game::proximityMultiplier = targetWeight * proximityMultiplierTarget +
                              (1 - targetWeight) * Game::proximityMultiplier;
  const float targetWeight2 = std::min(10.0f * timeDeltaSeconds, 1.f);
  Game::tintColor =
      targetWeight * tintTarget + (1 - targetWeight) * Game::tintColor;

  if (not Game::levelEnded && not spawnEvent && not collideEvent) {
    Game::levelEnded = true;
    Game::levelWon = true;
    if (proximityMultiplierTarget != 0.f)
      Audio::playSound(swoosh);
    tintTarget = glm::vec3(0, 1, 0);
    proximityMultiplierTarget = 0.f;
  }

  updateTubeMaterial(graphics, Game::tintColor);
}

} // namespace Game
