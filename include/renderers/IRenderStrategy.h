#pragma once

//#include <GLFW/glfw3.h>
struct GLFWwindow;

class Camera;
class GLFWwindow;

class IRenderStrategy {
public:
  virtual void Init() = 0;
  virtual void Render(int objectCount, Camera& camera, GLFWwindow *window) = 0;
  virtual void Cleanup() = 0;
  virtual const char *GetName() const = 0;

  virtual ~IRenderStrategy() = default;
private:
};
