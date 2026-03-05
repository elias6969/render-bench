#pragma once
#include <vector>

class BenchmarkSystem
{
public:
    void Update(float frameMs, float cpuMs);
    void Reset();

    float GetAverage() const;
    float GetMin() const;
    float GetMax() const;
    float GetStdDev() const;

    const std::vector<float>& GetHistory() const;

private:
    std::vector<float> m_FrameHistory;
    std::vector<float> m_CpuHistory;
};
