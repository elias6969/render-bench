#include "renderers/BatchRenderer.h"
#include "core/Camera.h"
#include "core/Cube.h"
#include "core/Shader.h"
#include "tools/EngineConfig.h"
#include "tools/TextureManager.h"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <random>
#include <vector>

void BatchRenderer::Init() {
  maxObjects = 50000;

  std::vector<float> batchedVertices;
  batchedVertices.reserve(maxObjects * 36 * 8);

  std::mt19937 rng(42);
  std::uniform_real_distribution<float> dist(-10.0f, 10.0f);

  // Build ALL cubes into one big buffer
  for (int i = 0; i < maxObjects; i++) {
    glm::vec3 position(dist(rng), dist(rng), dist(rng));
    glm::mat4 model = glm::translate(glm::mat4(1.0f), position);

    for (int v = 0; v < 36; v++) {
      int base = v * 8;

      glm::vec3 pos(cubeVertices[base + 0], cubeVertices[base + 1],
                    cubeVertices[base + 2]);

      glm::vec3 normal(cubeVertices[base + 3], cubeVertices[base + 4],
                       cubeVertices[base + 5]);

      glm::vec2 uv(cubeVertices[base + 6], cubeVertices[base + 7]);

      glm::vec3 transformed = glm::vec3(model * glm::vec4(pos, 1.0f));

      // Position
      batchedVertices.push_back(transformed.x);
      batchedVertices.push_back(transformed.y);
      batchedVertices.push_back(transformed.z);

      // Normal
      batchedVertices.push_back(normal.x);
      batchedVertices.push_back(normal.y);
      batchedVertices.push_back(normal.z);

      // UV
      batchedVertices.push_back(uv.x);
      batchedVertices.push_back(uv.y);
    }
  }

  totalVertices = maxObjects * 36;

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  glBufferData(GL_ARRAY_BUFFER, batchedVertices.size() * sizeof(float),
               batchedVertices.data(), GL_STATIC_DRAW);

  // Position
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);

  // Normal
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(3 * sizeof(float)));

  // UV
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                        (void *)(6 * sizeof(float)));

  glBindVertexArray(0);
  cube.texture = TextureLoader::loadTexture(
      (EngineConfig::TextureDirectory + "test1.jpg").c_str(), false);

  shader.LoadShaders((EngineConfig::ShaderDirectory + "batchcube.vs").c_str(),
                     (EngineConfig::ShaderDirectory + "basiccube.fs").c_str());

  std::cout << "Switch to Batch\n";
}

void BatchRenderer::Render(int objectCount, Camera &camera,
                           GLFWwindow *window) {
  shader.use();

  glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),
                                          static_cast<float>(EngineConfig::WindowWidth) /
                                              static_cast<float>(EngineConfig::WindowHeight),
                                          0.1f, 100.0f);

  glm::mat4 view = camera.GetViewMatrix();

  shader.setUniform("projection", projection);
  shader.setUniform("view", view);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, cube.texture);
  shader.setUniform("diffuseTex", 0);

  glBindVertexArray(VAO);

  int verticesToDraw = objectCount * 36;
  glDrawArrays(GL_TRIANGLES, 0, verticesToDraw);

  glBindVertexArray(0);
}

void BatchRenderer::Cleanup() {
  //Clean stuff here
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
}
