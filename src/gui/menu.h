#pragma once
namespace Graphics {
using Context = struct ContextT *;
}

namespace GUI {

void loadMenu(Graphics::Context graphics);
void unloadMenu(Graphics::Context graphics);
void updateMenu(Graphics::Context graphics);
void drawMenu(Graphics::Context graphics);

} // namespace GUI
