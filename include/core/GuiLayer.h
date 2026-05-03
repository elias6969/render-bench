#pragma once

#include "CSVWriter.h"
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
  int drawCalls{0};
  bool benchmark = false;
  double runtime = 0.0;
  CSVWriter csv;
  bool benchmarkStarted = false;

  int benchmarkRenderer = 0;
  int benchmarkObject = 1;

  float benchmarkTimer = 0.0f;
  float measureTimer = 0.0f;

  const float warmupTime = 0.5f;
  const float measureTime = 1.0f;

  float accumFPS = 0.0f;
  float accumCPU = 0.0f;
  float accumGPU = 0.0f;
  int sampleCount = 0;
  int currentStep = 0;
  const int totalSteps = 120;

public:
  void Init(GLFWwindow *window);

  void Render(PerformanceTracker &perf, int &objectCount, int &rendererIndex,
              bool &vsync);


  void BeginFrame();
  void RunBenchmark(PerformanceTracker& perf, int& objectCount, int& rendererIndex);
  void EndFrame(GLFWwindow* window);
  void Shutdown();
};
