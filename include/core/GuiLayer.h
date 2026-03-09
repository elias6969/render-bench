#pragma once

#include <GLFW/glfw3.h>

class GuiLayer {
private:
public:
  void Init(GLFWwindow *window);

  void Render(int &objectCount, int &rendererIndex, bool &vsync);

  void Shutdown();
};
