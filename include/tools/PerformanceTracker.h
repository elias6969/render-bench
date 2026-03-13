#pragma once

#include <array>

class PerformanceTracker {
public:
  static constexpr int HISTORY_SIZE = 120;

  void AddFrame(float frameTimeMs);
  void Update(int objectCount);

  float GetFPS() const { return fps; }
  float GetAvgFPS() const { return avgFPS; }
  float Get1Low() const { return fps1Low; }
  float Get01Low() const { return fps01Low; }

  float GetAvgFrame() const { return avgFrame; }
  float GetMinFrame() const { return minFrame; }
  float GetMaxFrame() const { return maxFrame; }
  float GetStdDev() const { return stddevFrame; }
  float GetJitter() const { return frameJitter; }
  float GetStability() const { return pacingStability; }

  float GetRuntime() const { return runtime; }
  int GetFrameCount() const { return frameCount; }

  const float *GetFrameHistory() const { return frameHistory.data(); }
  const float *GetFPSHistory() const { return fpsHistory.data(); }

private:
  std::array<float, HISTORY_SIZE> frameHistory{};
  std::array<float, HISTORY_SIZE> fpsHistory{};

  int historyIndex = 0;

  float fps = 0.0f;
  float avgFPS = 0.0f;
  float fps1Low = 0.0f;
  float fps01Low = 0.0f;

  float avgFrame = 0.0f;
  float minFrame = 0.0f;
  float maxFrame = 0.0f;
  float stddevFrame = 0.0f;
  float frameJitter = 0.0f;
  float pacingStability = 0.0f;

  float runtime = 0.0f;
  int frameCount = 0;
};
