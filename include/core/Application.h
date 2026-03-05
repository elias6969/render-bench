#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>

#include "core/Callbacks.h"
#include "core/Camera.h"
#include "renderers/IRenderStrategy.h"
#include "core/GuiLayer.h"

class Application {
public:
  Application();
  ~Application();

  void Run();

private:
  // Core systems
  GLFWwindow *m_Window = nullptr;
  Camera camera;
  Callbacks callbacks;
  std::unique_ptr<IRenderStrategy> m_Renderer;

  // Timing
  float deltaTime = 0.0f;
  float lastFrame = 0.0f;

  // App state
  int m_ObjectCount = 100;
  int m_CurrentRendererIndex = 0;
  bool m_VSync = false;

private:
  GuiLayer guilayer;
  void Init();
  void Shutdown();
  void Update();
  void Render();
  void SetupImGui();
};
