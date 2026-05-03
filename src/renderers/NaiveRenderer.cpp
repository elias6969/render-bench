#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "renderers/NaiveRenderer.h"
#include "core/Cube.h"
#include "tools/EngineConfig.h"

#include <glm/detail/qualifier.hpp>
#include <glm/fwd.hpp>
#include <random>



void NaiveRenderer::Init() 
{
  //init system
  cube.loadCube();
  positions.clear();

  std::mt19937 rng(42); // fixed seed
  std::uniform_real_distribution<float> dist(-10.0f, 10.0f);

  int maxObjects = 50000; // cap limit

  for (int i = 0; i < maxObjects; i++) {
    glm::vec3 pos(dist(rng), dist(rng), dist(rng));

    positions.push_back(pos);
  }
  std::cout << "Switch to Naive\n";
}

void NaiveRenderer::Render(int objectCount, Camera& camera, GLFWwindow *window) 
{
    cube.shader.use();

    glm::mat4 projection = glm::perspective(
        glm::radians(camera.Zoom),
        (float)EngineConfig::WindowWidth / (float)EngineConfig::WindowHeight,
        0.1f, 100.0f
    );

    glm::mat4 view = camera.GetViewMatrix();

    cube.shader.setUniform("projection", projection);
    cube.shader.setUniform("view", view);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, cube.texture);
    cube.shader.setUniform("diffuseTex", 0);

    glBindVertexArray(cube.VAO);

    for (int i = 0; i < objectCount && i < positions.size(); i++) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, positions[i]);

        cube.shader.setUniform("model", model);

        cube.drawRaw();
    }
}

void NaiveRenderer::Cleanup() 
{
  //Clean stuff here
}
