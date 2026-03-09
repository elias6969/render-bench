#pragma once

#include "core/Camera.h"
#include "tools/EngineConfig.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Callbacks {
public:
  // GLFW-required static callbacks
  static void framebuffer_size_callback(GLFWwindow *window, int width,
                                        int height);
  static void mouse_callback(GLFWwindow *window, double xpos, double ypos);
  static void scroll_callback(GLFWwindow *window, double xoffset,
                              double yoffset);

  // Regular input (called manually from Application::Update)
  void processInput(GLFWwindow *window, float deltaTime);

  // Runtime state
  bool cursorEnabled = true;
  bool firstMouse = true;
  float lastX = EngineConfig::WindowWidth * 0.5f;
  float lastY = EngineConfig::WindowHeight * 0.5f;

  Camera *camera = nullptr;
};
