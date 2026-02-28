#include "renderers/BatchRenderer.h"
#include "core/Camera.h"
#include "core/Cube.h"
#include "core/Shader.h"
#include "tools/EngineConfig.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/detail/qualifier.hpp>
#include <glm/fwd.hpp>
#include <random>

Cube cube;
std::vector<glm::vec3> positions;

void BatchRenderer::Init() {
  // init system
  cube.loadCube();
  positions.clear();

  std::mt19937 rng(42); // fixed seed = reproducible
  std::uniform_real_distribution<float> dist(-10.0f, 10.0f);

  int maxObjects = 50000; // cap limit

  for (int i = 0; i < maxObjects; i++) {
    glm::vec3 pos(dist(rng), dist(rng), dist(rng));

    positions.push_back(pos);
  }
}

void BatchRenderer::Render(int objectCount, Camera &camera,
                           GLFWwindow *window) {
  // Render here
  for (int i = 0; i < objectCount && i < positions.size(); i++) {
    cube.Position = positions[i];
    cube.render(camera, window);
  }
}

void BatchRenderer::Cleanup() {
}
