#include "text.h"
#include "graphics/graphics.h"

#include <SDL3_ttf/SDL_textengine.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <iostream>

namespace GUI {

namespace {

TTF_Font *font;
TTF_Font *smallFont;
TTF_TextEngine *engine;

} // namespace

void initTextRendering(Graphics::Context graphics) {
  font = TTF_OpenFont("./assets/font/mono-font.ttf", 100);
  if (not font) {
    std::cout << "Failed to load font\n";
    return;
  }
  smallFont = TTF_OpenFont("./assets/font/mono-font.ttf", 40);
  if (not smallFont) {
    std::cout << "Failed to load small font\n";
    return;
  }

  engine = TTF_CreateSurfaceTextEngine();

  if (not engine) {
    std::cout << "Failed to load text engine\n";
    return;
  }
}

TextSprite loadTextSprite(Graphics::Context graphics, Graphics::Texture texture,
                          const char *text, glm::vec3 color,
                          bool useSmallFont) {
  SDL_Color SDLColor{Uint8(color.x * 255), Uint8(color.y * 255),
                     Uint8(color.z * 255), 255};

  SDL_Surface *textSurface =
      TTF_RenderText_Solid(useSmallFont ? smallFont : font, text, 0, SDLColor);
  SDL_Surface *converted =
      SDL_ConvertSurface(textSurface, SDL_PIXELFORMAT_ABGR8888);
  Graphics::surfaceToTexture(graphics, converted, texture);
  TextSprite result{texture, textSurface->w, textSurface->h, useSmallFont,
                    color};
  SDL_DestroySurface(converted);
  SDL_DestroySurface(textSurface);

  return result;
}

void updateTextSprite(Graphics::Context graphics, TextSprite &sprite,
                      const char *text) {
  Graphics::destroyTexture(graphics, sprite.texture);
  sprite = loadTextSprite(graphics, sprite.texture, text, sprite.color,
                          sprite.useSmallFont);
}

} // namespace GUI
