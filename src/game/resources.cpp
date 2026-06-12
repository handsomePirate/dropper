#include "resources.h"
#include "graphics/primitives.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Game {

void loadTube(Graphics::Context graphics) {
  Graphics::allocateModels(graphics, 1, (Graphics::ModelT **)&tubeModel);
  Graphics::loadCylinderInv(graphics, tubeModel, 32);
  auto scale = glm::scale(glm::mat4(1),
                          glm::vec3(tubeDiameter, tubeLength, tubeDiameter));
  auto translate = glm::translate(glm::mat4(1), glm::vec3(0, -0.5, 0));
  tubeInstance = scale * translate;

  Graphics::allocateMaterials(graphics, 1,
                              (Graphics::MaterialT **)&tubeMaterial);
  Graphics::loadMaterial(graphics, tubeMaterial, "./tube.vert.spv",
                         "./tube.frag.spv", sizeof(TubeMaterialData));
  TubeMaterialData data{.color = glm::vec3{1}};
  Graphics::pushMaterialData(graphics, tubeMaterial, &data);

  Graphics::allocateInstanceBatches(
      graphics, 1, (Graphics::InstanceBatchT **)&tubeInstanceBatch);
  Graphics::createInstanceBatch(graphics, tubeInstanceBatch, tubeModel,
                                &tubeInstance, 1);
}

void destroyTube(Graphics::Context graphics) {
  Graphics::destroyInstanceBatch(graphics, tubeInstanceBatch);
  Graphics::freeInstanceBatches(graphics,
                                (Graphics::InstanceBatchT *)tubeInstanceBatch);

  Graphics::destroyMaterial(graphics, tubeMaterial);
  Graphics::freeMaterials(graphics, (Graphics::MaterialT *)tubeMaterial);

  Graphics::destroyModel(graphics, tubeModel);
  Graphics::freeModels(graphics, (Graphics::ModelT *)tubeModel);
}

} // namespace Game
