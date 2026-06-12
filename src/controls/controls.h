#pragma once
#include <glm/glm.hpp>

namespace Graphics {
using Context = struct ContextT *;
}

namespace Controls {

void startCameraMove();
void endCameraMove();

void handleKeyboard(bool &inMenu);
void handleMouse(bool &inMenu);

void keyboardMoveUpdate(float timeDeltaSeconds);
void mouseLookUpdate();
void simulatePlayerFall(float timeDeltaSeconds);

void setProximityMultipier(float proximityMultiplier);
void updateCamera(Graphics::Context graphics);

} // namespace Controls
