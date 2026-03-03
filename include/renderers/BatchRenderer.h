#pragma once

#include "IRenderStrategy.h"
#include "core/Cube.h"

class Shader;

class BatchRenderer : public IRenderStrategy {
public:
  BatchRenderer() = default;
  ~BatchRenderer() override = default;

  void Init() override;
  void Render(int objectCount, Camera &camera, GLFWwindow *window) override;
  void Cleanup() override;

  const char *GetName() const override { return "Batch"; }

private:
  Shader shader;
  Cube cube;
  GLuint VAO = 0;
  GLuint VBO = 0;

  int maxObjects = 50000;
  int totalVertices = 0;

  std::vector<glm::vec3> positions;


  unsigned int drawCalls = 0;
};
