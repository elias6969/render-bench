#pragma once

#include "IRenderStrategy.h"
#include <glm/fwd.hpp>
#include <vector>
#include "core/Cube.h"

class NaiveRenderer : public IRenderStrategy {
public:
  NaiveRenderer() = default;
  ~NaiveRenderer() override = default;

  void Init() override;
  void Render(int objectCount, Camera& camera, GLFWwindow *window) override;
  void Cleanup() override;

  const char *GetName() const override { return "Naive"; }

private:
  Cube cube;
  std::vector<glm::vec3> positions;
  unsigned int drawCalls = 0;
};
