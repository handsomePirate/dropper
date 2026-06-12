#include "controls.h"
#include "game/level.h"
#include "game/resources.h"
#include "graphics/graphics.h"

#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Controls {

// globals with internal linkage
namespace {

int windowWidth, windowHeight;
float mouseX;
float mouseY;
const glm::vec3 cameraBaseForward = {0, -1, 0};
const glm::vec3 cameraBaseUp = {0, 0, -1};
const glm::vec3 cameraBaseRight = {1, 0, 0};
glm::vec3 cameraForward = cameraBaseForward;
glm::vec3 cameraRight = cameraBaseRight;
glm::vec3 cameraUp = cameraBaseUp;
glm::quat cameraRot = glm::quat(1, 0, 0, 0);
glm::vec3 movementForce;

glm::vec3 cameraPos;

} // namespace

void startCameraMove() { Game::lastCameraPosition = Game::cameraPosition; }

void endCameraMove() {
  Game::cameraVelocity = Game::cameraPosition - Game::lastCameraPosition;
}

void handleKeyboard(bool &inMenu) {
  if (not inMenu) {
    movementForce = glm::vec3{};
    auto states = SDL_GetKeyboardState(nullptr);
    bool forward = false;
    bool other = false;

    if (states[SDL_SCANCODE_A] || states[SDL_SCANCODE_LEFT]) {
      movementForce -= cameraRight;
      other = true;
    }
    if (states[SDL_SCANCODE_D] || states[SDL_SCANCODE_RIGHT]) {
      movementForce += cameraRight;
      other = true;
    }
    if (states[SDL_SCANCODE_S] || states[SDL_SCANCODE_DOWN]) {
      movementForce -= cameraUp;
      other = true;
    }
    if (states[SDL_SCANCODE_W] || states[SDL_SCANCODE_UP]) {
      movementForce += cameraUp;
      forward = true;
    }

    float mult = 1.f;
    if (forward and not other)
      mult = 1.2f;

    movementForce = glm::normalize(movementForce) * mult;
    if (std::isnan(movementForce.x))
      movementForce = glm::vec3{};
  } else {
    inMenu = false;
  }
}

void handleMouse(bool &inMenu) {
  if (inMenu) {
    inMenu = false;
  }
}

void keyboardMoveUpdate(float timeDeltaSeconds) {
  if (not Game::levelEnded) {
    Game::cameraPosition +=
        movementForce * Game::playerSpeed * timeDeltaSeconds;

    glm::vec2 camera2D{Game::cameraPosition.x, Game::cameraPosition.z};
    auto distFromCenter = glm::distance(camera2D, glm::vec2{});
    auto radius = Game::tubeDiameter / 2;
    if (distFromCenter + Game::playerSize > radius) {
      auto camera2DScaled =
          glm::normalize(camera2D) * (radius - Game::playerSize);
      Game::cameraPosition.x = camera2DScaled.x;
      Game::cameraPosition.z = camera2DScaled.y;
    }
  }
}

void mouseLookUpdate() {
  float x, y;
  SDL_GetRelativeMouseState(&x, &y);

  const float rotationSpeed = 0.001f;

  auto yaw = glm::angleAxis(x * rotationSpeed, glm::vec3(0, -1, 0));
  auto pitch = glm::angleAxis(/*y * rotationSpeed*/ 0.f, glm::vec3(1, 0, 0));

  cameraRot = yaw * cameraRot * pitch;

  cameraForward = cameraRot * cameraBaseForward;
  cameraRight = cameraRot * cameraBaseRight;
  cameraUp = cameraRot * cameraBaseUp;
}

void simulatePlayerFall(float timeDeltaSeconds) {
  if (not Game::levelEnded or Game::levelWon) {
    Game::cameraSpeed += Game::cameraSpeedIncrease * timeDeltaSeconds;
    Game::cameraSpeedIncrease -= std::max(0.05f * timeDeltaSeconds, 0.f);
    Game::playerSpeed += Game::playerSpeedIncrease * timeDeltaSeconds;
    Game::playerSpeedIncrease -= std::max(0.08f * timeDeltaSeconds, 0.f);
    Game::cameraPosition.y -= Game::cameraSpeed * timeDeltaSeconds;
  }
}

void updateCamera(Graphics::Context graphics) {
  glm::mat4 view = glm::lookAt(Game::cameraPosition,
                               Game::cameraPosition + cameraForward, cameraUp);
  auto window = Graphics::getWindow(graphics);

  int w, h;
  SDL_GetWindowSize(window, &w, &h);

  if (windowWidth != w or windowHeight != h) {
    windowWidth = w;
    windowHeight = h;
    Graphics::updateViewport(graphics);
  }

  glm::mat4 projection =
      glm::perspective(glm::radians(45.0f), w / float(h), 0.001f, 10000.0f);
  Graphics::GlobalData data{.vp = projection * view,
                            .pos = Game::cameraPosition,
                            .proximityMultiplier = Game::proximityMultiplier};
  Graphics::updateGlobal(graphics, &data);
}

} // namespace Controls
