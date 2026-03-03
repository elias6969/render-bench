#ifndef CUBE_H
#define CUBE_H

#include "Camera.h"
#include "Material.h"
#include "Shader.h"
#include "Material.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

class Cube {
public:
  glm::vec3 Position;
  glm::vec3 Rotation;
  float Alpha;
  float time;
  glm::vec3 size;
  float r, g, b;
  const char *texturePath;

  void loadCube();
  void render(Camera &camera, GLFWwindow *window);
  ~Cube();

  GLuint VAO, VBO, texture;
  Shader shader;
  Material material;
private:
};

extern float cubeVertices[];

#endif
