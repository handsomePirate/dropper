#pragma once
#include "graphics/graphics.h"

namespace Graphics {

void loadCube(Context graphics, Model model);
void loadCylinder(Context graphics, Model model, uint32_t detail);
void loadCylinderInv(Context graphics, Model model, uint32_t detail);
void loadSquare(Context graphics, Model model);
void loadCircle(Context graphics, Model model, uint32_t detail);

} // namespace Graphics
