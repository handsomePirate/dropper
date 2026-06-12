#pragma once
#include <stdint.h>

namespace Game {
struct Level;
}

namespace Graphics {
using Context = struct ContextT *;
}

namespace Levels {

void loadLevelAssets(Graphics::Context graphics);
void unloadLevelAssets(Graphics::Context graphics);
bool loadLevel(Graphics::Context graphics, Game::Level *level, uint32_t i);
void unloadLevel(Graphics::Context graphics, Game::Level *level);
void drawLevel(Graphics::Context graphics, Game::Level *level);

} // namespace Levels
