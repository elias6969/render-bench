#include "tools/PerformanceTracker.h"

#include <GLFW/glfw3.h>
#include <algorithm>
#include <cmath>
#include <imgui.h>
#include <numeric>
#include <vector>

void PerformanceTracker::AddFrame(float frameTimeMs) {
  frameHistory[historyIndex] = frameTimeMs;
  fpsHistory[historyIndex] = 1000.0f / frameTimeMs;

  historyIndex = (historyIndex + 1) % HISTORY_SIZE;
}

void PerformanceTracker::Update(int objectCount) {
  float currentFrame = static_cast<float>(glfwGetTime());

  deltaTime = currentFrame - lastFrame;
  lastFrame = currentFrame;
  avgFrame = std::accumulate(frameHistory.begin(), frameHistory.end(), 0.0f) /
             HISTORY_SIZE;

  minFrame = *std::min_element(frameHistory.begin(), frameHistory.end());
  maxFrame = *std::max_element(frameHistory.begin(), frameHistory.end());

  float variance = 0.0f;

  for (float t : frameHistory)
    variance += (t - avgFrame) * (t - avgFrame);

  variance /= HISTORY_SIZE;

  stddevFrame = std::sqrt(variance);

  fps = 1000.0f / avgFrame;
  avgFPS = fps;

  std::vector<float> sorted(frameHistory.begin(), frameHistory.end());
  std::sort(sorted.begin(), sorted.end());

  int idx1 = static_cast<int>(HISTORY_SIZE * 0.99f);
  int idx01 = static_cast<int>(HISTORY_SIZE * 0.999f);

  fps1Low = 1000.0f / sorted[idx1];
  fps01Low = 1000.0f / sorted[std::min(idx01, HISTORY_SIZE - 1)];

  frameJitter = stddevFrame;

  pacingStability = (1.0f - (stddevFrame / avgFrame)) * 100.0f;

  runtime = ImGui::GetTime();
  frameCount++;
}
