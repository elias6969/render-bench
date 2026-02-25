#pragma once

class IRenderStrategy {
public:
  virtual void Init() = 0;
  virtual void Render(int objectCount) = 0;
  virtual void Cleanup() = 0;
  virtual const char *GetName() const = 0;

  virtual ~IRenderStrategy() = default;
private:
};
