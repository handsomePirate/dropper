#pragma once
#include <glm/glm.hpp>

namespace Graphics {
using Context = struct ContextT *;
using Texture = struct TextureT *;
} // namespace Graphics

namespace GUI {

void initTextRendering(Graphics::Context graphics);
struct TextSprite {
  Graphics::Texture texture;
  int w, h;
  bool useSmallFont;
  glm::vec3 color;
};
// texture must be allocated
TextSprite loadTextSprite(Graphics::Context graphics, Graphics::Texture texture,
                          const char *text, glm::vec3 color, bool useSmallFont);
void updateTextSprite(Graphics::Context graphics, TextSprite &sprite,
                      const char *text);

} // namespace GUI
