#pragma once

#include <glm/glm.hpp>

namespace Graphics {
using Context = struct ContextT *;
}

namespace Game {

inline bool gameEnded;

void reset(Graphics::Context graphics);
void load(Graphics::Context graphics);
void unload(Graphics::Context graphics);
void update(Graphics::Context graphics, float deltaSeconds);
void draw(Graphics::Context graphics);

} // namespace Game
