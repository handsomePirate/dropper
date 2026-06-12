#include "graphics.h"

#include <GL/glew.h>
#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include <filesystem>
#include <fstream>
#include <sstream>

namespace Graphics {

namespace {

void checkGLErrors(const char *label) {
  GLenum err;
  while ((err = glGetError()) != GL_NO_ERROR) {
    printf("GL Error at %s: %d\n", label, err);
  }
}

GLuint compileShader(const char *source, GLenum type) {
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &source, nullptr);
  glCompileShader(shader);

  // Check for compilation errors
  GLint success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    char log[512];
    glGetShaderInfoLog(shader, 512, nullptr, log);
    SDL_LogError(SDL_LOG_CATEGORY_RENDER, "Shader compilation failed: %s", log);
    glDeleteShader(shader);
    return 0;
  }

  return shader;
}

GLuint compileShaderBinary(const uint8_t *bin, uint32_t size, GLenum type) {
  GLuint shader = glCreateShader(type);
  glShaderBinary(1, &shader, GL_SHADER_BINARY_FORMAT_SPIR_V_ARB, bin, size);
  glSpecializeShaderARB(shader, "main", 0, 0, 0);

  // Check for compilation errors
  GLint success;
  glGetShaderiv(shader, GL_SPIR_V_BINARY_ARB, &success);
  if (not success) {
    char log[512];
    glGetShaderInfoLog(shader, 512, nullptr, log);
    SDL_LogError(SDL_LOG_CATEGORY_RENDER, "Shader specialization failed: %s",
                 log);
    glDeleteShader(shader);
    return 0;
  }

  return shader;
}

GLuint linkProgram(GLuint vertex, GLuint fragment) {
  GLuint program = glCreateProgram();
  glAttachShader(program, vertex);
  glAttachShader(program, fragment);
  glLinkProgram(program);

  // Check for linking errors
  GLint success;
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success) {
    char log[512];
    glGetProgramInfoLog(program, 512, nullptr, log);
    SDL_LogError(SDL_LOG_CATEGORY_RENDER, "Shader linking failed: %s", log);
    glDeleteProgram(program);
    return 0;
  }

  glDeleteShader(vertex);
  glDeleteShader(fragment);

  return program;
}

} // namespace

struct ModelT {
  uint32_t vertexCount;
  uint32_t indexCount;
  GLuint VAO;
  GLuint posVBO;
  GLuint normVBO;
  GLuint uvVBO;
  GLuint indexVBO;
};

struct MaterialT {
  GLuint shaderProgram;
  GLuint UBO;
  uint32_t usize;
};

struct InstanceBatchT {
  Model model;
  GLuint transformVBO;
  uint32_t instanceCount;
};

struct TextureT {
  uint32_t index;
  int w, h;
};

struct ContextT {
  SDL_Window *window;
  SDL_GLContext GLContext;

  GLuint spriteVAO;
  GLuint spriteVertexVBO;
  GLuint spriteInstanceVBO;
  GLuint spriteSampler;
  MaterialT spriteMaterial;

  GLuint cameraBuffer;
};

size_t getModelTSize() { return sizeof(ModelT); }

Model getModel(ModelT *models, uint32_t i) { return models + i; }

size_t getMaterialTSize() { return sizeof(MaterialT); }

Material getMaterial(MaterialT *materials, uint32_t i) { return materials + i; }

size_t getInstanceBatchTSize() { return sizeof(InstanceBatchT); }

InstanceBatch getInstanceBatch(InstanceBatchT *instanceBatches, uint32_t i) {
  return instanceBatches + i;
}

size_t getTextureTSize() { return sizeof(TextureT); }

Texture getTexture(TextureT *textures, uint32_t i) { return textures + i; }

namespace {

std::string readFile(const std::filesystem::path &path) {
  std::ifstream file(path);
  std::stringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}
std::vector<uint8_t> readFileBinary(const std::filesystem::path &path) {
  std::ifstream file(path, std::ios::binary);
  if (not file) {
    return {};
  }

  // Get file size
  file.seekg(0, std::ios::end);
  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);

  // Resize vector and read
  std::vector<uint8_t> res(size);
  if (not file.read((char *)res.data(), size)) {
    return {};
  }

  return res;
}

void setupSpriteVAO(Context context) {
  float vertices[] = {
      0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
      1.0f, 0.0f, 1.0f, 1.0f, // bottom-right
      0.0f, 1.0f, 0.0f, 0.0f, // top-left
      1.0f, 1.0f, 1.0f, 0.0f, // top-right
  };

  glGenVertexArrays(1, &context->spriteVAO);
  glBindVertexArray(context->spriteVAO);

  GLuint buffers[2];
  glGenBuffers(2, buffers);
  context->spriteVertexVBO = buffers[0];
  context->spriteInstanceVBO = buffers[1];

  // VERTEX
  glBindBuffer(GL_ARRAY_BUFFER, context->spriteVertexVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                        (void *)(2 * sizeof(float)));

  // INSTANCE x, y, w, h
  glBindBuffer(GL_ARRAY_BUFFER, context->spriteInstanceVBO);
  const int maxSprites = 16;
  glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * maxSprites, nullptr,
               GL_DYNAMIC_DRAW);

  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), nullptr);
  glVertexAttribDivisor(2, 1);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void destroySpriteVAO(Context context) {
  GLuint buffers[2];
  buffers[0] = context->spriteVertexVBO;
  buffers[1] = context->spriteInstanceVBO;
  glDeleteBuffers(2, buffers);
  glDeleteVertexArrays(1, &context->spriteVAO);
}

void initSpriteSampler(Context context) {
  glGenSamplers(1, &context->spriteSampler);

  glSamplerParameteri(context->spriteSampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glSamplerParameteri(context->spriteSampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glSamplerParameteri(context->spriteSampler, GL_TEXTURE_WRAP_S,
                      GL_CLAMP_TO_EDGE);
  glSamplerParameteri(context->spriteSampler, GL_TEXTURE_WRAP_T,
                      GL_CLAMP_TO_EDGE);
}

void destroySpriteSampler(Context context) {
  glDeleteSamplers(1, &context->spriteSampler);
}

} // namespace

Context init(SDL_Window *window) {
  if (not window) {
    std::cout << "SDL window not supplied\n";
    return nullptr;
  }

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
  auto GLContext = SDL_GL_CreateContext(window);

  if (not GLContext) {
    std::cout << "SDL GL context creation failed\n";
    return nullptr;
  }

  SDL_GL_MakeCurrent(window, GLContext);
  SDL_GL_SetSwapInterval(1);

  GLenum err = glewInit();
  if (GLEW_OK != err) {
    SDL_GL_DestroyContext(GLContext);
    std::cout << glewGetErrorString(err) << '\n';
    return nullptr;
  }

  auto context = new ContextT;
  context->window = window;
  int w, h;
  SDL_GetWindowSize(window, &w, &h);
  context->GLContext = GLContext;

  glGenBuffers(1, &context->cameraBuffer);
  glBindBuffer(GL_UNIFORM_BUFFER, context->cameraBuffer);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(GlobalData), nullptr, GL_DYNAMIC_DRAW);
  setupSpriteVAO(context);
  loadMaterial(context, &context->spriteMaterial, "./sprite.vert.spv",
               "./sprite.frag.spv", 0);
  initSpriteSampler(context);

  glEnable(GL_CULL_FACE);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  return context;
}

void shutdown(Context context) {
  if (!context)
    return;

  destroySpriteSampler(context);
  destroyMaterial(context, &context->spriteMaterial);
  destroySpriteVAO(context);
  glDeleteBuffers(1, &context->cameraBuffer);

  SDL_GL_DestroyContext(context->GLContext);
  delete context;
}

SDL_Window *getWindow(Context context) { return context->window; }

void allocateModels(Context context, uint32_t count, ModelT **models) {
  *models = new ModelT[count];
}

void freeModels(Context context, ModelT *models) { delete[] models; }

void loadModel(Context context, Model model, float pos[], float norm[],
               float uv[], uint32_t vsize, uint32_t ind[], uint32_t isize) {
  if (not context || not model || not pos || not norm || not uv || not ind ||
      vsize == 0 || isize == 0) {
    return;
  }

  // TODO: Check the data whether the model seems already initialized
  // model

  model->vertexCount = vsize;
  model->indexCount = isize;

  GLuint buffers[4];
  glGenBuffers(4, buffers);
  model->posVBO = buffers[0];
  model->normVBO = buffers[1];
  model->uvVBO = buffers[2];
  model->indexVBO = buffers[3];

  glGenVertexArrays(1, &model->VAO);
  glBindVertexArray(model->VAO);

  // POSITION
  glBindBuffer(GL_ARRAY_BUFFER, model->posVBO);
  glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(float) * vsize, pos, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
  glVertexAttribDivisor(0, 0);
  glObjectLabel(GL_BUFFER, model->posVBO, -1, "positions");

  // NORMAL
  glBindBuffer(GL_ARRAY_BUFFER, model->normVBO);
  glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(float) * vsize, norm,
               GL_STATIC_DRAW);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 3 * sizeof(float), nullptr);
  glVertexAttribDivisor(1, 0);
  glObjectLabel(GL_BUFFER, model->normVBO, -1, "normals");

  // UV
  glBindBuffer(GL_ARRAY_BUFFER, model->uvVBO);
  glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(float) * vsize, uv, GL_STATIC_DRAW);
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_TRUE, 2 * sizeof(float), nullptr);
  glVertexAttribDivisor(2, 0);
  glObjectLabel(GL_BUFFER, model->uvVBO, -1, "uvs");

  // INDEX
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->indexVBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * isize, ind,
               GL_STATIC_DRAW);
  glObjectLabel(GL_BUFFER, model->indexVBO, -1, "indexes");

  glBindVertexArray(0);

  checkGLErrors("[load model]");
}

void destroyModel(Context context, Model model) {
  if (not context || not model) {
    return;
  }

  GLuint buffers[4];
  buffers[0] = model->indexVBO;
  buffers[1] = model->uvVBO;
  buffers[2] = model->normVBO;
  buffers[3] = model->posVBO;
  glDeleteBuffers(4, buffers);

  glDeleteVertexArrays(1, &model->VAO);

  checkGLErrors("[destroy model]");
}

void allocateMaterials(Context context, uint32_t count, MaterialT **materials) {
  *materials = new MaterialT[count];
}

void freeMaterials(Context context, MaterialT *materials) {
  delete[] materials;
}

void loadMaterial(Context context, Material material, const char *vertSpvFile,
                  const char *fragSpvFile, uint32_t uniformSize) {
  if (not context || not material || not vertSpvFile || not fragSpvFile) {
    return;
  }

  auto vertexShader = readFileBinary(vertSpvFile);
  auto fragmentShader = readFileBinary(fragSpvFile);

  auto vs = compileShaderBinary(vertexShader.data(), vertexShader.size(),
                                GL_VERTEX_SHADER);
  auto fs = compileShaderBinary(fragmentShader.data(), fragmentShader.size(),
                                GL_FRAGMENT_SHADER);
  if (not vs || not fs) {
    if (vs)
      glDeleteShader(vs);
    if (fs)
      glDeleteShader(fs);
    return;
  }

  material->shaderProgram = linkProgram(vs, fs);
  if (not material->shaderProgram) {
    return;
  }

  if (uniformSize > 0) {
    glGenBuffers(1, &material->UBO);
    glBindBuffer(GL_UNIFORM_BUFFER, material->UBO);
    glBufferData(GL_UNIFORM_BUFFER, uniformSize, nullptr, GL_DYNAMIC_DRAW);
  }

  material->usize = uniformSize;

  checkGLErrors("[load material]");
}

void pushMaterialData(Context context, Material material, void *data,
                      uint32_t offset, uint32_t size) {
  if (not context || not material) {
    return;
  }

  glBindBuffer(GL_UNIFORM_BUFFER, material->UBO);
  size = size == 0 ? material->usize - offset
                   : std::min(size, material->usize - offset);
  glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);

  checkGLErrors("[push material data]");
}

void destroyMaterial(Context context, Material material) {
  if (not context || not material) {
    return;
  }

  if (material->usize > 0)
    glDeleteBuffers(1, &material->UBO);
  material->usize = 0;

  glDeleteProgram(material->shaderProgram);

  checkGLErrors("[destroy material]");
}

void allocateInstanceBatches(Context context, uint32_t count,
                             InstanceBatchT **instanceBatches) {
  *instanceBatches = new InstanceBatchT[count];
}

void freeInstanceBatches(Context context, InstanceBatchT *instanceBatches) {
  delete[] instanceBatches;
}

void createInstanceBatch(Context context, InstanceBatch instanceBatch,
                         Model model, glm::mat4 *instances,
                         uint32_t instanceCount) {
  if (not context || not instanceBatch || not instances || instanceCount == 0) {
    return;
  }

  instanceBatch->model = model;
  instanceBatch->instanceCount = instanceCount;

  glGenBuffers(1, &instanceBatch->transformVBO);
  glBindBuffer(GL_COPY_WRITE_BUFFER, instanceBatch->transformVBO);
  glBufferData(GL_COPY_WRITE_BUFFER, instanceCount * sizeof(glm::mat4), nullptr,
               GL_STATIC_DRAW);

  for (int i = 0; i < instanceCount; ++i) {
    glBufferSubData(GL_COPY_WRITE_BUFFER, i * sizeof(glm::mat4),
                    sizeof(glm::mat4), &instances[i]);
  }

  checkGLErrors("[create instance batch]");
}

void destroyInstanceBatch(Context context, InstanceBatch instanceBatch) {
  glDeleteBuffers(1, &instanceBatch->transformVBO);
  instanceBatch->transformVBO = 0;
}

void allocateTextures(Context context, uint32_t count, TextureT **textures) {
  *textures = new TextureT[count];
}

void freeTextures(Context context, TextureT *textures) { delete[] textures; }

void surfaceToTexture(Context context, SDL_Surface *surface, Texture texture) {
  if (not surface || not texture) {
    return;
  }

  GLuint index;
  glGenTextures(1, &index);
  glBindTexture(GL_TEXTURE_2D, index);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  GLenum format;
  GLenum revformat;
  GLenum reversion;
  if (surface->format == SDL_PIXELFORMAT_BGR24) {
    format = GL_RGB8;
    revformat = GL_BGR;
    reversion = GL_UNSIGNED_BYTE;
  } else if (surface->format == SDL_PIXELFORMAT_ABGR8888) {
    format = GL_RGBA8;
    revformat = GL_RGBA;
    reversion = GL_UNSIGNED_INT_8_8_8_8_REV;
  } else {
    assert(false);
  }

  glTexImage2D(GL_TEXTURE_2D, 0, format, surface->w, surface->h, 0, revformat,
               reversion, surface->pixels);

  glBindTexture(GL_TEXTURE_2D, 0);

  texture->index = index;
  texture->w = surface->w;
  texture->h = surface->h;
  checkGLErrors("[surface to texture]");
}

void destroyTexture(Context context, Texture texture) {
  glDeleteTextures(1, &texture->index);
}

void getTextureSize(Context context, Texture texture, int *w, int *h) {
  *w = texture->w;
  *h = texture->h;
}

void beginFrame(Context context, const glm::vec3 &clearColor) {
  if (not context)
    return;

  SDL_GL_MakeCurrent(context->window, context->GLContext);

  glClearColor(clearColor.r, clearColor.g, clearColor.b, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void drawInstanceBatch(Context context, InstanceBatch instanceBatch,
                       Material material) {
  if (not context || not material || not instanceBatch) {
    return;
  }

  const ModelT *model = instanceBatch->model;

  glEnable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);
  glUseProgram(material->shaderProgram);
  // Bind general camera data
  glBindBufferBase(GL_UNIFORM_BUFFER, 0, context->cameraBuffer);
  // Bind material data
  if (material->usize > 0)
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, material->UBO);
  // Bind VAO
  glBindVertexArray(model->VAO);
  // Bind instance transforms
  glBindBuffer(GL_ARRAY_BUFFER, instanceBatch->transformVBO);
  for (int i = 0; i < 4; i++) {
    glEnableVertexAttribArray(3 + i);
    glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, 16 * sizeof(float),
                          (const void *)(i * 4 * sizeof(float)));
    glVertexAttribDivisor(3 + i, 1);
  }
  glDrawElementsInstanced(GL_TRIANGLES, model->indexCount, GL_UNSIGNED_INT,
                          nullptr, instanceBatch->instanceCount);
  glBindVertexArray(0);

  checkGLErrors("[draw instance batch]");
}

void drawSprite(Context context, Texture texture, int x, int y, int sx,
                int sy) {
  if (not context || not texture) {
    return;
  }

  int w, h;
  SDL_GetWindowSize(context->window, &w, &h);

  float fx = x / float(w);
  float fy = y / float(h);
  float fsx = (sx == 0 ? texture->w : sx) / float(w);
  float fsy = (sy == 0 ? texture->h : sy) / float(h);

  glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glm::vec4 data(fx, fy, fsx, fsy);
  glBindBuffer(GL_COPY_WRITE_BUFFER, context->spriteInstanceVBO);
  glBufferSubData(GL_COPY_WRITE_BUFFER, 0, sizeof(glm::vec4), &data);

  const MaterialT &material = context->spriteMaterial;
  glUseProgram(material.shaderProgram);

  glBindVertexArray(context->spriteVAO);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture->index);
  glBindSampler(0, context->spriteSampler);

  glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, 1);
  glBindVertexArray(0);

  checkGLErrors("[draw sprite]");
}

void endFrame(Context context) {
  if (not context)
    return;

  SDL_GL_SwapWindow(context->window);
}

void updateViewport(Context context) {
  if (not context)
    return;

  SDL_GL_MakeCurrent(context->window, context->GLContext);
  int w, h;
  SDL_GetWindowSize(context->window, &w, &h);
  glViewport(0, 0, w, h);
}

void updateGlobal(Context context, const GlobalData *data) {
  if (not context)
    return;

  glBindBuffer(GL_UNIFORM_BUFFER, context->cameraBuffer);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(GlobalData), data);
}

} // namespace Graphics
