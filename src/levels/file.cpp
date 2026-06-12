#include "file.h"
#include "game/level.h"
#include "graphics/graphics.h"
#include "graphics/primitives.h"
#include "levels/slab-event.h"

#include <fstream>
#include <iostream>
#include <memory>
#include <string>

namespace Levels {

namespace {

Graphics::Model slabModel;
Graphics::Material obstacleMaterial;

struct LevelObstacleMaterialData {
  glm::vec3 color;
  float maxDist;
};

} // namespace

void loadLevelAssets(Graphics::Context graphics) {
  Graphics::allocateModels(graphics, 1, (Graphics::ModelT **)&slabModel);
  Graphics::loadCube(graphics, slabModel);

  Graphics::allocateMaterials(graphics, 1,
                              (Graphics::MaterialT **)&obstacleMaterial);
  Graphics::loadMaterial(graphics, obstacleMaterial, "./obstacle.vert.spv",
                         "./obstacle.frag.spv",
                         sizeof(LevelObstacleMaterialData));

  LevelObstacleMaterialData materialData{.color = glm::vec3{1},
                                         .maxDist = Game::tubeLength};
  Graphics::pushMaterialData(graphics, obstacleMaterial, &materialData);
}

void unloadLevelAssets(Graphics::Context graphics) {
  Graphics::destroyMaterial(graphics, obstacleMaterial);
  Graphics::freeMaterials(graphics, obstacleMaterial);

  Graphics::destroyModel(graphics, slabModel);
  Graphics::freeModels(graphics, slabModel);
}

bool loadLevel(Graphics::Context graphics, Game::Level *level, uint32_t i) {
  level->spawnIndex = 0;
  level->collideIndex = 0;

  std::string levelName = std::format("level{}", i);
  std::ifstream ifs(std::format("./assets/levels/{}", levelName));
  if (not ifs.is_open()) {
    return false;
  }

  std::string line;
  while (std::getline(ifs, line)) {
    if (line[1] == '0') {
      level->eventQueue.push_back(std::make_unique<SlabEvent>(
          graphics, slabModel, obstacleMaterial, line.c_str()));
    } else if (line[1] == '1') {
      level->eventQueue.push_back(std::make_unique<SlabCombinationEvent>(
          graphics, slabModel, obstacleMaterial, line.c_str()));
    } else {
      std::cout << "Unknown event in " << levelName << '\n';
      return false;
    }
  }

  return true;
}

void unloadLevel(Graphics::Context graphics, Game::Level *level) {
  level->eventQueue.clear();
}

void drawLevel(Graphics::Context graphics, Game::Level *level) {
  for (int i = std::max(level->collideIndex - 1, 0u); i < level->spawnIndex;
       ++i) {
    level->eventQueue[i]->draw(graphics);
  }
}

} // namespace Levels
