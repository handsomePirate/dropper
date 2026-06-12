#include "game.h"
#include "audio/sound.h"
#include "controls/controls.h"
#include "game/level.h"
#include "game/resources.h"
#include "graphics/graphics.h"
#include "gui/menu.h"
#include "gui/text.h"
#include "levels/file.h"

#include <SDL3/SDL.h>
#include <glm/gtc/matrix_transform.hpp>

#include <format>
#include <string>

namespace Game {

// globals with internal linkage
namespace {

float lastTimeSeconds;
float endCountdown;
int levelIndex;
GUI::TextSprite levelText;
Graphics::Texture levelTextTexture;

Game::Level level;

void resetInternals() {
  cameraPosition = glm::vec3{};
  tintColor = glm::vec3{1};
  cameraSpeed = 0.8f;
  cameraSpeedIncrease = 4.2f;
  playerSpeed = 1.0f;
  playerSpeedIncrease = 0.5f;
  proximityMultiplier = 0.f;
  levelEnded = false;
  levelWon = false;
  endCountdown = 2.f;
}

void loadGameLevel(Graphics::Context graphics) {
  resetInternals();

  std::string levelName = std::format("level {}", levelIndex);
  Game::menuResultText = std::format("You failed on level {}.", levelIndex);

  Levels::unloadLevel(graphics, &level);
  if (not Levels::loadLevel(graphics, &level, levelIndex)) {
    Game::menuResultText = std::format("You completed the game!");
    gameEnded = true;
    return;
  }
  GUI::updateTextSprite(graphics, levelText, levelName.c_str());

  loadLevel(graphics, &level);
}

} // namespace

void reset(Graphics::Context graphics) {
  levelIndex = 1;
  loadGameLevel(graphics);
}

void load(Graphics::Context graphics) {
  Graphics::allocateModels(graphics, 1, (Graphics::ModelT **)&fallbackMaterial);
  Graphics::loadMaterial(graphics, fallbackMaterial, "./fallback.vert.spv",
                         "./fallback.frag.spv", sizeof(FallbackMaterialData));
  updateFallbackMaterial(graphics, glm::vec3{1});

  Graphics::allocateTextures(graphics, 1,
                             (Graphics::TextureT **)&levelTextTexture);
  levelText = GUI::loadTextSprite(graphics, levelTextTexture, "placeholder",
                                  glm::vec3{1, 0, 0}, false);
  gasp = Audio::loadSound("./assets/audio/gasp.wav");
  crash = Audio::loadSound("./assets/audio/crash.wav");
  swoosh = Audio::loadSound("./assets/audio/swoosh.wav");

  loadTube(graphics);

  Levels::loadLevelAssets(graphics);

  gameEnded = false;
}

void unload(Graphics::Context graphics) {
  Levels::unloadLevelAssets(graphics);

  destroyTube(graphics);

  Graphics::destroyTexture(graphics, levelTextTexture);
  Graphics::freeTextures(graphics, levelTextTexture);

  Graphics::destroyMaterial(graphics, fallbackMaterial);
  Graphics::freeMaterials(graphics, fallbackMaterial);

  Audio::unloadSound(swoosh);
  Audio::unloadSound(crash);
  Audio::unloadSound(gasp);
}

void update(Graphics::Context graphics, float deltaSeconds) {
  updateLevel(graphics, &level, deltaSeconds);
  if (levelEnded)
    endCountdown -= deltaSeconds;

  if (endCountdown <= 0) {
    if (levelWon) {
      ++levelIndex;
      loadGameLevel(graphics);
    } else {
      gameEnded = true;
    }
  }
}

void draw(Graphics::Context graphics) {
  auto window = Graphics::getWindow(graphics);
  int w, h;
  SDL_GetWindowSize(window, &w, &h);
  Graphics::beginFrame(graphics, glm::vec3{});
  drawTube(graphics);
  Levels::drawLevel(graphics, &level);
  Graphics::drawSprite(graphics, levelText.texture, w - levelText.w,
                       h - levelText.h, levelText.w, levelText.h);
  Graphics::endFrame(graphics);
}

} // namespace Game
