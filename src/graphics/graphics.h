#pragma once
#include <glm/glm.hpp>

#include <stdint.h>

struct SDL_Window;
struct SDL_Surface;

namespace Graphics {

//
// DATA
//
using Context = struct ContextT *;

struct ModelT;
using Model = ModelT *;
size_t getModelTSize();
Model getModel(ModelT *models, uint32_t i);
struct MaterialT;
using Material = MaterialT *;
size_t getMaterialTSize();
Material getMaterial(MaterialT *materials, uint32_t i);
struct InstanceBatchT;
using InstanceBatch = InstanceBatchT *;
size_t getInstanceBatchTSize();
InstanceBatch getInstanceBatch(InstanceBatchT *instanceBatches, uint32_t i);
struct TextureT;
using Texture = TextureT *;
size_t getTextureTSize();
Texture getTexture(TextureT *textures, uint32_t i);

//
// Model is the internal ModelT*, but handling specifically ModelT* indicates
// operations on a physical model array (similarly for other objects).
// Model is an opaque handle that also just happends to be a pointer to ModelT.
//

//
// MANAGEMENT
//
Context init(SDL_Window *window);
void shutdown(Context context);
SDL_Window *getWindow(Context context);

//
// MODEL
//
void allocateModels(Context context, uint32_t count, ModelT **models);
void freeModels(Context context, ModelT *models);
void loadModel(Context context, Model model, float pos[], float norm[],
               float uv[], uint32_t vsize, uint32_t ind[], uint32_t isize);
void destroyModel(Context context, Model model);

//
// MATERIAL
//
void allocateMaterials(Context context, uint32_t count, MaterialT **materials);
void freeMaterials(Context context, MaterialT *materials);
void loadMaterial(Context context, Material material, const char *vertSpvFile,
                  const char *fragSpvFile, uint32_t uniformSize);
void pushMaterialData(Context context, Material material, void *data,
                      uint32_t offset = 0, uint32_t size = 0);
void destroyMaterial(Context context, Material material);

//
// INSTANCE BATCH
//
void allocateInstanceBatches(Context context, uint32_t count,
                             InstanceBatchT **instanceBatches);
void freeInstanceBatches(Context context, InstanceBatchT *instanceBatches);
void createInstanceBatch(Context context, InstanceBatch instanceBatch,
                         Model model, glm::mat4 *instances,
                         uint32_t instanceCount);
void destroyInstanceBatch(Context context, InstanceBatch instanceBatch);

//
// TEXTURE
//
void allocateTextures(Context context, uint32_t count, TextureT **textures);
void freeTextures(Context context, TextureT *textures);
void surfaceToTexture(Context context, SDL_Surface *surface, Texture texture);
void destroyTexture(Context context, Texture texture);
void getTextureSize(Context context, Texture texture, int *w, int *h);

//
// DRAWING
//
void beginFrame(Context context, const glm::vec3 &clearColor);
void drawInstanceBatch(Context context, InstanceBatch instanceBatch,
                       Material material);
void drawSprite(Context context, Texture texture, int x, int y, int sx = 0,
                int sy = 0);
void endFrame(Context context);

//
// CAMERA
//
void updateViewport(Context context);
struct GlobalData {
  glm::mat4 vp;
  glm::vec3 pos;
  float proximityMultiplier;
};
void updateGlobal(Context context, const GlobalData *data);

} // namespace Graphics
