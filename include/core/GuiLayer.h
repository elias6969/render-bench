#pragma once

#include "tools/PerformanceTracker.h"
#include <GLFW/glfw3.h>
#include <array>

class GuiLayer {
private:

  static constexpr int HISTORY_SIZE = 200;

  std::array<float, HISTORY_SIZE> frameHistory{};
  std::array<float, HISTORY_SIZE> fpsHistory{};

  int historyIndex = 0;
  int frameCount = 0;
  double runtime = 0.0;

public:
  void Init(GLFWwindow *window);

  void Render(PerformanceTracker &perf, int &objectCount, int &rendererIndex, bool &vsync);

  void Shutdown();
};
