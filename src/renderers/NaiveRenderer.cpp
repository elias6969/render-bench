#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "renderers/NaiveRenderer.h"
#include "core/Cube.h"
#include "core/Shader.h"

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
  //Render here
  for (int i = 0; i < objectCount && i < positions.size(); i++) {
    cube.Position = positions[i];
    cube.render(camera, window);
  }
}

void NaiveRenderer::Cleanup() 
{
  //Clean stuff here
}
