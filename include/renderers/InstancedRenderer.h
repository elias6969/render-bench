#pragma once

#include "IRenderStrategy.h"
#include <glm/fwd.hpp>
#include <vector>
#include "core/Cube.h"

class Shader;
class Mesh;
class Camera;
class GLFWwindow;

class InstancedRenderer : public IRenderStrategy {
public:
  InstancedRenderer() = default;
  ~InstancedRenderer() override = default;

  void Init() override;
  void Render(int objectCount, Camera &camera, GLFWwindow *window) override;
  void Cleanup() override;

  const char *GetName() const override { return "Instance"; }

private:
  Cube cube;
  std::vector<glm::vec3> positions;
  Shader *shader;
  Mesh *mesh;
  int maxObjects{0};
  GLuint instanceVBO, instanceVAO;

  unsigned int drawCalls = 0;
};
