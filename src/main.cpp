#include "audio/sound.h"
#include "controls/controls.h"
#include "game/game.h"
#include "graphics/graphics.h"
#include "gui/menu.h"
#include "gui/text.h"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include <chrono>
#include <iostream>

namespace {

glm::vec3 lerp(const glm::vec3 &v1, const glm::vec3 &v2, float val) {
  return (1.f - val) * v1 + val * v2;
}

auto start = std::chrono::high_resolution_clock::now();
float getTimeSeconds() {
  return std::chrono::duration_cast<std::chrono::microseconds>(
             std::chrono::high_resolution_clock::now() - start)
             .count() *
         0.000001f;
}

} // namespace

int main() {
  if (not SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
    std::cout << "Could not initialize SDL\n";
    return 1;
  }

  if (not TTF_Init()) {
    std::cout << "Could not initialize SDL_ttf\n";
    return 1;
  }

  auto window = SDL_CreateWindow("window", 1600, 1000, SDL_WINDOW_OPENGL);
  // SDL_CaptureMouse(true);

  auto graphics = Graphics::init(window);

  if (not graphics) {
    std::cout << "Failed to create graphics context\n";
    return 1;
  }

  GUI::initTextRendering(graphics);
  GUI::loadMenu(graphics);
  Audio::init();
  Game::load(graphics);

  // float colorRamp = 0.f;
  bool shouldEnd = false;
  float lastTimeSeconds = getTimeSeconds();
  float timeDeltaSeconds = 0.0f;
  bool inMenu = true;

  uint32_t updates = 0;
  uint32_t lastSeconds = 0;
  uint32_t seconds = 0;
  while (not shouldEnd) {

    float timeSeconds = getTimeSeconds();

    bool prevInMenu = inMenu;

    if (not inMenu && Game::gameEnded) {
      inMenu = true;
    }

    bool resized = false;
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EventType::SDL_EVENT_KEY_DOWN) {
        if (event.key.key == SDLK_ESCAPE) {
          shouldEnd = true;
        }
      }

      if (event.type == SDL_EventType::SDL_EVENT_QUIT) {
        shouldEnd = true;
      }

      if (event.type == SDL_EventType::SDL_EVENT_WINDOW_RESIZED) {
        resized = true;
      }

      if (event.type == SDL_EventType::SDL_EVENT_KEY_DOWN) {
        Controls::handleKeyboard(inMenu);
      } else if (event.type == SDL_EventType::SDL_EVENT_KEY_UP) {
        Controls::handleKeyboard(inMenu);
      } else if (event.type == SDL_EventType::SDL_EVENT_MOUSE_BUTTON_DOWN) {
        Controls::handleMouse(inMenu);
      } else if (event.type == SDL_EventType::SDL_EVENT_MOUSE_BUTTON_UP) {
        Controls::handleMouse(inMenu);
      }
    }

    if (shouldEnd) {
      break;
    }

    if (prevInMenu && not inMenu) {
      SDL_SetWindowRelativeMouseMode(window, true);
      Game::reset(graphics);
      Game::gameEnded = false;
    } else if (not prevInMenu && inMenu) {
      SDL_SetWindowRelativeMouseMode(window, false);
      GUI::updateMenu(graphics);
    }

    // TODO: this should ideally be tied to the monitor's refresh rate
    const auto target = 0.0166666675f;
    if (inMenu) {
      GUI::drawMenu(graphics);
    } else {
      Controls::startCameraMove();

      Controls::keyboardMoveUpdate(timeDeltaSeconds);
      Controls::mouseLookUpdate();
      Controls::simulatePlayerFall(timeDeltaSeconds);

      Controls::endCameraMove();

      Game::update(graphics, timeDeltaSeconds);
      Controls::updateCamera(graphics);

      // stabilize framerate (before vsync to avoid jitter)
      float frameSeconds = getTimeSeconds();
      auto delta = frameSeconds - timeSeconds;
      if (delta < target) {
        auto delay = (target - delta) * 1000.f;
        SDL_Delay(delay);
      }
      Game::draw(graphics);
    }

    float delayedSeconds = getTimeSeconds();
    timeDeltaSeconds = std::min(delayedSeconds - timeSeconds, target);
  }

  Game::unload(graphics);
  GUI::unloadMenu(graphics);
  Audio::close();
  Graphics::shutdown(graphics);
  SDL_DestroyWindow(window);
  SDL_Quit();

  std::flush(std::cout);

  return 0;
}
