#include "menu.h"
#include "game/resources.h"
#include "graphics/graphics.h"
#include "gui/text.h"

#include <SDL3/SDL.h>

namespace GUI {

namespace {

TextSprite title;
Graphics::Texture titleTexture;
TextSprite result;
Graphics::Texture resultTexture;
TextSprite instruction;
Graphics::Texture instructionTexture;

} // namespace

void loadMenu(Graphics::Context graphics) {
  Graphics::allocateTextures(graphics, 1, (Graphics::TextureT **)&titleTexture);
  Graphics::allocateTextures(graphics, 1,
                             (Graphics::TextureT **)&resultTexture);
  Graphics::allocateTextures(graphics, 1,
                             (Graphics::TextureT **)&instructionTexture);

  title = GUI::loadTextSprite(graphics, titleTexture, "Dropper",
                              glm::vec3{1, 0, 0}, false);
  result =
      GUI::loadTextSprite(graphics, resultTexture, Game::menuResultText.c_str(),
                          glm::vec3{0.2, 0.3f, 1}, true);
  instruction = GUI::loadTextSprite(graphics, instructionTexture,
                                    "Press any key to play (ESC to quit)...",
                                    glm::vec3{1, 0, 0}, true);
}

void unloadMenu(Graphics::Context graphics) {
  Graphics::destroyTexture(graphics, titleTexture);
  Graphics::freeTextures(graphics, titleTexture);

  Graphics::destroyTexture(graphics, resultTexture);
  Graphics::freeTextures(graphics, resultTexture);

  Graphics::destroyTexture(graphics, instructionTexture);
  Graphics::freeTextures(graphics, instructionTexture);
}

void updateMenu(Graphics::Context graphics) {
  GUI::updateTextSprite(graphics, result, Game::menuResultText.c_str());
}

void drawMenu(Graphics::Context graphics) {
  auto window = Graphics::getWindow(graphics);
  int w, h;
  SDL_GetWindowSize(window, &w, &h);

  Graphics::beginFrame(graphics, glm::vec3{});
  Graphics::drawSprite(graphics, title.texture, w / 2 - title.w / 2,
                       h / 2 - title.h / 2 + title.h, title.w, title.h);
  Graphics::drawSprite(graphics, result.texture, w / 2 - result.w / 2,
                       h / 2 - result.h, result.w, result.h);
  Graphics::drawSprite(graphics, instruction.texture, w / 2 - instruction.w / 2,
                       h / 2 - instruction.h / 2 - title.h, instruction.w,
                       instruction.h);
  Graphics::endFrame(graphics);
}

} // namespace GUI
