#include "core/Application.h"
#include "core/Callbacks.h"
#include "tools/EngineConfig.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <memory>

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <numeric>
#include <vector>

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>

#include "core/Application.h"
#include "core/Callbacks.h"
#include "core/Camera.h"

#include "renderers/BatchRenderer.h"
#include "renderers/IRenderStrategy.h"
#include "renderers/InstancedRenderer.h"
#include "renderers/NaiveRenderer.h"

Application::Application() { std::cout << "Application::Started\n"; }

void Application::Init() {
  std::cout << "Init::Started\n";

  EngineConfig::WindowWidth = 1920;
  EngineConfig::WindowHeight = 1080;

  if (!glfwInit()) {
    std::cout << "GLFW Init Failed\n";
    return;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_ALPHA_BITS, 8);
  glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);

  m_Window =
      glfwCreateWindow(EngineConfig::WindowWidth, EngineConfig::WindowHeight,
                       "GL-Bench", nullptr, nullptr);

  if (!m_Window) {
    std::cout << "Window Creation Failed\n";
    glfwTerminate();
    return;
  }

  glfwMakeContextCurrent(m_Window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "GLAD Init Failed\n";
    glfwTerminate();
    return;
  }

  glEnable(GL_DEPTH_TEST);

  // ---- Setup Callbacks ----
  glfwSetWindowUserPointer(m_Window, &callbacks);

  glfwSetFramebufferSizeCallback(m_Window,
                                 Callbacks::framebuffer_size_callback);
  glfwSetCursorPosCallback(m_Window, Callbacks::mouse_callback);
  glfwSetScrollCallback(m_Window, Callbacks::scroll_callback);

  glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  guilayer.Init(m_Window);

  // Inject camera into callback system
  callbacks.camera = &camera;

  m_Renderer = std::make_unique<NaiveRenderer>();
  m_Renderer->Init();
}

void Application::Shutdown() {
  m_Renderer->Cleanup();
  guilayer.Shutdown();
  glfwTerminate();
  std::cout << "Shutdown::Application\n";
}

void Application::Update() {
  float currentFrame = static_cast<float>(glfwGetTime());
  deltaTime = currentFrame - lastFrame;
  lastFrame = currentFrame;

  callbacks.processInput(m_Window, deltaTime);
}

void Application::Render() {
  float currentFrame = glfwGetTime();
  deltaTime = currentFrame - lastFrame;
  lastFrame = currentFrame;
  callbacks.processInput(m_Window, deltaTime);

  glClearColor(0.1f, 0.1f, 0.5f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  static int lastRenderer = -1;

  if (m_CurrentRendererIndex != lastRenderer) {
    if (m_Renderer)
      m_Renderer->Cleanup();

    switch (m_CurrentRendererIndex) {
    case 0:
      m_Renderer = std::make_unique<NaiveRenderer>();
      break;
    case 1:
      m_Renderer = std::make_unique<InstancedRenderer>();
      break;
    case 2:
      m_Renderer = std::make_unique<BatchRenderer>();
      break;
    }

    m_Renderer->Init();
    lastRenderer = m_CurrentRendererIndex;
  }

  // render stuff here
  m_Renderer->Render(m_ObjectCount, camera, m_Window);

  guilayer.Render(m_ObjectCount, m_CurrentRendererIndex, m_VSync);

  glfwSwapBuffers(m_Window);
  glfwPollEvents();
}

void Application::Run() {
  Init();

  while (!glfwWindowShouldClose(m_Window)) {
    Update();
    Render();
  }

  Shutdown();
}

Application::~Application() {}
