#include "primitives.h"

#include <assert.h>
#include <vector>

namespace Graphics {

void loadCube(Context graphics, Model model) {
  glm::vec3 pos[] = {
      // Front face
      {-0.5f, -0.5f, 0.5f},
      {0.5f, -0.5f, 0.5f},
      {0.5f, 0.5f, 0.5f},
      {-0.5f, 0.5f, 0.5f},
      // Back face
      {-0.5f, -0.5f, -0.5f},
      {0.5f, -0.5f, -0.5f},
      {0.5f, 0.5f, -0.5f},
      {-0.5f, 0.5f, -0.5f},
      // Top face
      {-0.5f, 0.5f, -0.5f},
      {0.5f, 0.5f, -0.5f},
      {0.5f, 0.5f, 0.5f},
      {-0.5f, 0.5f, 0.5f},
      // Bottom face
      {-0.5f, -0.5f, -0.5f},
      {0.5f, -0.5f, -0.5f},
      {0.5f, -0.5f, 0.5f},
      {-0.5f, -0.5f, 0.5f},
      // Right face
      {0.5f, -0.5f, -0.5f},
      {0.5f, 0.5f, -0.5f},
      {0.5f, 0.5f, 0.5f},
      {0.5f, -0.5f, 0.5f},
      // Left face
      {-0.5f, -0.5f, -0.5f},
      {-0.5f, 0.5f, -0.5f},
      {-0.5f, 0.5f, 0.5f},
      {-0.5f, -0.5f, 0.5f},
  };

  glm::vec3 norm[] = {
      // Front face
      {0, 0, 1},
      {0, 0, 1},
      {0, 0, 1},
      {0, 0, 1},
      // Back face
      {0, 0, -1},
      {0, 0, -1},
      {0, 0, -1},
      {0, 0, -1},
      // Top face
      {0, 1, 0},
      {0, 1, 0},
      {0, 1, 0},
      {0, 1, 0},
      // Bottom face
      {0, -1, 0},
      {0, -1, 0},
      {0, -1, 0},
      {0, -1, 0},
      // Right face
      {1, 0, 0},
      {1, 0, 0},
      {1, 0, 0},
      {1, 0, 0},
      // Left face
      {-1, 0, 0},
      {-1, 0, 0},
      {-1, 0, 0},
      {-1, 0, 0},
  };

  glm::vec2 uv[] = {
      // Front face
      {0, 0},
      {1, 0},
      {1, 1},
      {0, 1},
      // Back face
      {1, 0},
      {0, 0},
      {0, 1},
      {1, 1},
      // Top face
      {0, 0},
      {1, 0},
      {1, 1},
      {0, 1},
      // Bottom face
      {1, 0},
      {0, 0},
      {0, 1},
      {1, 1},
      // Right face
      {0, 0},
      {0, 1},
      {1, 1},
      {1, 0},
      // Left face
      {1, 0},
      {1, 1},
      {0, 1},
      {0, 0},
  };

  uint32_t ind[] = {
      0,  1,  2,  2,  3,  0,  // Front
      4,  6,  5,  4,  7,  6,  // Back
      8,  11, 10, 10, 9,  8,  // Top
      12, 13, 14, 14, 15, 12, // Bottom
      16, 17, 18, 18, 19, 16, // Right
      20, 23, 22, 22, 21, 20  // Left
  };

  Graphics::loadModel(graphics, model, (float *)pos, (float *)norm, (float *)uv,
                      sizeof(pos) / sizeof(glm::vec3), ind,
                      sizeof(ind) / sizeof(uint32_t));
}

namespace {

void createCylinderGeometry(std::vector<glm::vec3> &pos,
                            std::vector<glm::vec3> &norm,
                            std::vector<glm::vec2> &uv, uint32_t detail,
                            float normSign) {
  const float twoPi = 2.0f * std::numbers::pi_v<float>;
  for (int i = 0; i < detail; ++i) {
    float u = i / float(detail - 1);
    float rad = u * twoPi;
    float x = cos(rad) * 0.5f;
    float y = sin(rad) * 0.5f;
    pos[i] = {x, -0.5f, y};
    pos[i + detail] = {x, 0.5f, y};
    norm[i] = {normSign * x, 0, normSign * y};
    norm[i + detail] = {normSign * x, 0, normSign * y};
    uv[i] = {u, 0};
    uv[i + detail] = {u, 1};
  }
}

} // namespace

void loadCylinder(Context graphics, Model model, uint32_t detail) {
  assert(detail > 3);

  std::vector<glm::vec3> pos(detail * 2);
  std::vector<glm::vec3> norm(detail * 2);
  std::vector<glm::vec2> uv(detail * 2);
  std::vector<uint32_t> ind((detail - 1) * 6);

  createCylinderGeometry(pos, norm, uv, detail, 1.f);

  for (int i = 0; i < detail - 1; ++i) {
    int i1 = i;
    int i2 = i + 1;
    int i3 = i + detail;
    int i4 = i + 1 + detail;
    ind[i * 6 + 0] = i1;
    ind[i * 6 + 1] = i3;
    ind[i * 6 + 2] = i2;
    ind[i * 6 + 3] = i3;
    ind[i * 6 + 4] = i4;
    ind[i * 6 + 5] = i2;
  }

  Graphics::loadModel(graphics, model, (float *)pos.data(),
                      (float *)norm.data(), (float *)uv.data(), pos.size(),
                      ind.data(), ind.size());
}

void loadCylinderInv(Context graphics, Model model, uint32_t detail) {
  assert(detail >= 3);

  std::vector<glm::vec3> pos(detail * 2);
  std::vector<glm::vec3> norm(detail * 2);
  std::vector<glm::vec2> uv(detail * 2);
  std::vector<uint32_t> ind((detail - 1) * 6);

  createCylinderGeometry(pos, norm, uv, detail, -1.f);

  for (int i = 0; i < detail - 1; ++i) {
    int i1 = i;
    int i2 = i + 1;
    int i3 = i + detail;
    int i4 = i + 1 + detail;
    ind[i * 6 + 0] = i1;
    ind[i * 6 + 1] = i2;
    ind[i * 6 + 2] = i3;
    ind[i * 6 + 3] = i3;
    ind[i * 6 + 4] = i2;
    ind[i * 6 + 5] = i4;
  }

  Graphics::loadModel(graphics, model, (float *)pos.data(),
                      (float *)norm.data(), (float *)uv.data(), pos.size(),
                      ind.data(), ind.size());
}

void loadSquare(Context graphics, Model model) {
  glm::vec3 pos[] = {
      {-0.5f, 0, -0.5f}, {0.5f, 0, -0.5f}, {0.5f, 0, 0.5f}, {-0.5f, 0, 0.5f}};
  glm::vec3 norm[] = {{0, 1, 0}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0}};
  glm::vec2 uv[] = {{0, 0}, {1, 0}, {1, 1}, {0, 1}};
  uint32_t ind[] = {0, 2, 1, 0, 3, 2};

  Graphics::loadModel(graphics, model, (float *)pos, (float *)norm, (float *)uv,
                      sizeof(pos) / sizeof(glm::vec3), ind,
                      sizeof(ind) / sizeof(uint32_t));
}

void loadCircle(Context graphics, Model model, uint32_t detail) {
  assert(detail > 3);

  std::vector<glm::vec3> pos(detail + 1);
  std::vector<glm::vec3> norm(detail + 1);
  std::vector<glm::vec2> uv(detail + 1);
  std::vector<uint32_t> ind((detail - 1) * 3);

  const float twoPi = 2.0f * std::numbers::pi_v<float>;
  for (int i = 0; i < detail; ++i) {
    float u = i / float(detail - 1);
    float rad = u * twoPi;
    float x = cos(rad) * 0.5f;
    float y = sin(rad) * 0.5f;
    pos[i] = {x, 0.f, y};
    norm[i] = {0, 1, 0};
    uv[i] = {x + 0.5f, y + 0.5f};
  }

  pos[detail] = {0, 0, 0};
  norm[detail] = {0, 1, 0};
  uv[detail] = {0.5f, 0.5f};

  for (int i = 0; i < detail - 1; ++i) {
    int i1 = i;
    int i2 = i + 1;
    int i3 = detail;
    ind[i * 3 + 0] = i1;
    ind[i * 3 + 1] = i3;
    ind[i * 3 + 2] = i2;
  }

  Graphics::loadModel(graphics, model, (float *)pos.data(),
                      (float *)norm.data(), (float *)uv.data(), pos.size(),
                      ind.data(), ind.size());
}

} // namespace Graphics
