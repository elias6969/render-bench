#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "core/Camera.h"
#include "core/Cube.h"
#include "tools/EngineConfig.h"
#include "renderers/InstancedRenderer.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <random>

void InstancedRenderer::Init() {
  cube.loadCube();

  positions.clear();

  std::mt19937 rng(42);
  std::uniform_real_distribution<float> dist(-10.0f, 10.0f);

  maxObjects = 50000;

  for (int i = 0; i < maxObjects; i++) {
    positions.emplace_back(dist(rng), dist(rng), dist(rng));
  }

  std::vector<glm::mat4> modelMatrices;
  modelMatrices.reserve(maxObjects);

  for (int i = 0; i < maxObjects; i++) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, positions[i]);
    modelMatrices.push_back(model);
  }

  glGenBuffers(1, &instanceVBO);
  glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
  glBufferData(GL_ARRAY_BUFFER, modelMatrices.size() * sizeof(glm::mat4),
               modelMatrices.data(), GL_STATIC_DRAW);

  glBindVertexArray(cube.VAO);

  std::size_t vec4Size = sizeof(glm::vec4);

  for (int i = 0; i < 4; i++) {
    glEnableVertexAttribArray(3 + i);
    glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
                          (void *)(i * vec4Size));

    glVertexAttribDivisor(3 + i, 1); // <-- magic line
  }

  glBindVertexArray(0);
  cube.shader.LoadShaders((EngineConfig::ShaderDirectory + "instancecube.vs").c_str(),
                     (EngineConfig::ShaderDirectory + "basiccube.fs").c_str());
  std::cout << "Swith to Naive\n";
}

void InstancedRenderer::Render(int objectCount, Camera &camera,
                               GLFWwindow *window) {
  cube.shader.use();

  glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom),
                                          static_cast<float>(EngineConfig::WindowWidth) / static_cast<float>(EngineConfig::WindowHeight), 0.1f, 100.0f);

  glm::mat4 view = camera.GetViewMatrix();

  cube.shader.setUniform("projection", projection);
  cube.shader.setUniform("view", view);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, cube.texture);
  cube.shader.setUniform("diffuseTex", 0);

  glBindVertexArray(cube.VAO);

  glDrawArraysInstanced(GL_TRIANGLES, 0, 36, objectCount);
}

void InstancedRenderer::Cleanup() {
  // Clean stuff here
  glDeleteVertexArrays(1, &cube.VAO);
  glDeleteBuffers(1, &cube.VBO);
}
