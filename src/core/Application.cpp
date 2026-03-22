#include "core/Application.h"
#include "CSVWriter.h"
#include "core/Callbacks.h"
#include "tools/EngineConfig.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>

#include <chrono>
#include <iostream>
#include <memory>

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>

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
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  m_Window =
      glfwCreateWindow(EngineConfig::WindowWidth, EngineConfig::WindowHeight,
                       "GL-Bench", nullptr, nullptr);

  if (!m_Window) {
    std::cout << "Window Creation Failed\n";
    glfwTerminate();
    return;
  }

  glfwMakeContextCurrent(m_Window);
  glfwSwapInterval(0);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "GLAD Init Failed\n";
    glfwTerminate();
    return;
  }

  glEnable(GL_DEPTH_TEST);

  // Callbacks
  glfwSetWindowUserPointer(m_Window, &callbacks);

  glfwSetFramebufferSizeCallback(m_Window,
                                 Callbacks::framebuffer_size_callback);

  glfwSetCursorPosCallback(m_Window, Callbacks::mouse_callback);

  glfwSetScrollCallback(m_Window, Callbacks::scroll_callback);

  glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  // GUI
  guilayer.Init(m_Window);

  callbacks.camera = &camera;

  // Renderer 
  m_Renderer = std::make_unique<NaiveRenderer>();
  m_Renderer->Init();

  // GPU timer query
  glGenQueries(1, &gpuQuery);
  CSVWriter csv;
  csv.newRow() << "this" << "is" << "the" << "first" << "row";
  csv.newRow() << "this" << "is" << "the" << "second" << "row";
  csv.writeToFile("foobar.csv");
}

void Application::Shutdown() {
  if (m_Renderer)
    m_Renderer->Cleanup();

  glDeleteQueries(1, &gpuQuery);

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
  float frameStart = glfwGetTime();

  glClearColor(0.1f, 0.1f, 0.5f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  static int lastRenderer = -1;

  // Switch renderer
  if (m_CurrentRendererIndex != lastRenderer) {
    if (m_Renderer)
      m_Renderer->Cleanup();

    switch (m_CurrentRendererIndex) {
    case 0:
      m_Renderer = std::make_unique<NaiveRenderer>();
      isNaive = true;
      break;

    case 1:
      m_Renderer = std::make_unique<InstancedRenderer>();
      isNaive = false;
      break;

    case 2:
      m_Renderer = std::make_unique<BatchRenderer>();
      isNaive = false;
      break;
    }

    m_Renderer->Init();
    lastRenderer = m_CurrentRendererIndex;
  }

  // CPU timer start
  auto cpuStart = std::chrono::high_resolution_clock::now();

  // GPU timer start
  glBeginQuery(GL_TIME_ELAPSED, gpuQuery);

  m_Renderer->Render(m_ObjectCount, camera, m_Window);

  glEndQuery(GL_TIME_ELAPSED);

  auto cpuEnd = std::chrono::high_resolution_clock::now();

  // CPU time
  float cpuTimeMs =
      std::chrono::duration<float, std::milli>(cpuEnd - cpuStart).count();

  // GPU time
  GLuint64 gpuTimeNs = 0;
  glGetQueryObjectui64v(gpuQuery, GL_QUERY_RESULT, &gpuTimeNs);

  float gpuTimeMs = gpuTimeNs / 1000000.0f;

  // GUI
  guilayer.Render(perf, m_ObjectCount, m_CurrentRendererIndex, m_VSync);

  glfwSwapBuffers(m_Window);
  glfwPollEvents();

  // Frame time
  float frameEnd = glfwGetTime();
  float frameTimeMs = (frameEnd - frameStart) * 1000.0f;

  perf.AddFrame(frameTimeMs);
  perf.Update(m_ObjectCount);

  perf.SetCPUTime(cpuTimeMs);
  perf.SetGPUTime(gpuTimeMs);
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
