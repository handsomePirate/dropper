#pragma once
#include "audio/sound.h"
#include "graphics/graphics.h"

#include <string>

namespace Game {

inline glm::vec3 cameraPosition{};
inline glm::vec3 lastCameraPosition{};
inline glm::vec3 cameraVelocity{};
inline float cameraSpeed;
inline float cameraSpeedIncrease;
inline float playerSpeed;
inline float playerSpeedIncrease;
inline float proximityMultiplier;
inline bool levelEnded;
inline bool levelWon;
inline glm::vec3 tintColor;

inline Audio::Sound gasp;
inline Audio::Sound crash;
inline Audio::Sound swoosh;

inline std::string menuResultText = "You haven't played yet.";

//
// GENERAL FALLBACK MATERIAL
//
inline Graphics::Material fallbackMaterial = nullptr;
struct FallbackMaterialData {
  glm::vec3 color;
  float _padding1;
};
inline void updateFallbackMaterial(Graphics::Context graphics,
                                   const glm::vec3 &color) {
  FallbackMaterialData data{.color = color};
  Graphics::pushMaterialData(graphics, fallbackMaterial, &data);
}

//
// TUBE
//
inline Graphics::Model tubeModel = nullptr;
inline glm::mat4 tubeInstance;
inline Graphics::Material tubeMaterial = nullptr;
inline Graphics::InstanceBatch tubeInstanceBatch = nullptr;
static constexpr float tubeDiameter = 20.f;
static constexpr float tubeLength = 100.f;
struct TubeMaterialData {
  glm::vec3 color;
  float _padding1;
};
void loadTube(Graphics::Context graphics);
void destroyTube(Graphics::Context graphics);
inline void updateTubeMaterial(Graphics::Context graphics,
                               const glm::vec3 &color) {
  TubeMaterialData data{.color = tintColor};
  Graphics::pushMaterialData(graphics, tubeMaterial, &data);
}
inline void drawTube(Graphics::Context graphics) {
  Graphics::drawInstanceBatch(graphics, tubeInstanceBatch, tubeMaterial);
}

} // namespace Game
