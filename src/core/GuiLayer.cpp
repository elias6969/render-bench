#include "core/GuiLayer.h"
#include "tools/EngineConfig.h"
#include "tools/PerformanceTracker.h"

#include "imguiThemes.h"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>

#include <CSVWriter.h>

#include <glm/detail/qualifier.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

static const char *rendererNames[] = {"Naive", "Instanced", "Batch"};

// INIT
void GuiLayer::Init(GLFWwindow *window) {
#if REMOVE_IMGUI == 0
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  imguiThemes::green();

  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

  io.FontGlobalScale = 1.0f;

  ImGuiStyle &style = ImGui::GetStyle();
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    style.Colors[ImGuiCol_WindowBg].w = 0.0f;
    style.Colors[ImGuiCol_DockingEmptyBg].w = 0.0f;
  }

  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 330");
#endif

  // Benchmark defaults
  benchmarkRenderer = 0;
  benchmarkObject = 1;
  currentStep = 0;
}

// FRAME BEGIN
void GuiLayer::BeginFrame() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
}

// MAIN UI
void GuiLayer::Render(PerformanceTracker &perf, int &objectCount,
                      int &rendererIndex, bool &vsync) {
  ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
  ImGui::Begin("Benchmark");

  // Controls
  ImGui::SliderInt("Object Count", &objectCount, 1, 50000);

  ImGui::Combo("Renderer", &rendererIndex, rendererNames,
               IM_ARRAYSIZE(rendererNames));

  drawCalls = (rendererIndex == 0) ? objectCount : 1;

  if (ImGui::Checkbox("VSync", &vsync))
    glfwSwapInterval(vsync ? 1 : 0);

  ImGui::Separator();

  // Frame Stats
  ImGui::Text("CPU Frame Time: %.3f ms", perf.GetCPUTime());
  ImGui::Text("GPU Frame Time: %.3f ms", perf.GetGPUTime());
  ImGui::Text("FPS: %.1f", perf.GetFPS());
  ImGui::Text("Avg FPS: %.1f", perf.GetAvgFPS());

  ImGui::Text("1%% Low: %.1f", perf.Get1Low());
  ImGui::Text("0.1%% Low: %.1f", perf.Get01Low());

  ImGui::Separator();

  ImGui::Text("Frame Avg: %.3f ms", perf.GetAvgFrame());
  ImGui::Text("Frame Min: %.3f ms", perf.GetMinFrame());
  ImGui::Text("Frame Max: %.3f ms", perf.GetMaxFrame());

  ImGui::Text("StdDev: %.3f ms", perf.GetStdDev());
  ImGui::Text("Jitter: %.3f ms", perf.GetJitter());
  ImGui::Text("Frame Stability: %.1f%%", perf.GetStability());

  ImGui::Separator();

  // Throughput
  float objectsPerSecond = objectCount * perf.GetAvgFPS();

  ImGui::Text("Objects: %d", objectCount);
  ImGui::Text("Objects/sec: %.0f", objectsPerSecond);
  ImGui::Text("DrawCalls: %d", drawCalls);

  ImGui::Separator();

  // Runtime
  ImGui::Text("Runtime: %.1f sec", perf.GetRuntime());
  ImGui::Text("Frames: %d", perf.GetFrameCount());

  ImGui::Separator();

  // Graphs
  ImGui::PlotLines("Frame Time (ms)", perf.GetFrameHistory(),
                   PerformanceTracker::HISTORY_SIZE);
  ImGui::PlotLines("FPS", perf.GetFPSHistory(),
                   PerformanceTracker::HISTORY_SIZE);

  ImGui::Separator();

  // ImGui Stats
  ImGuiIO &io = ImGui::GetIO();

  ImGui::Text("ImGui Vertices: %d", io.MetricsRenderVertices);
  ImGui::Text("ImGui Indices: %d", io.MetricsRenderIndices);
  ImGui::Text("ImGui Draw Calls: %d", io.MetricsRenderWindows);

  ImGui::Separator();

  // Benchmark Controls
  if (ImGui::Button("Run Benchmark")) {
    benchmark = true;
    benchmarkStarted = false;
  }

  if (ImGui::Button("Stop Benchmark")) {
    benchmark = false;
    csv.writeToFile("benchmark.csv");
  }

  RunBenchmark(perf, objectCount, rendererIndex);
  ImGui::PopStyleColor();
  ImGui::End();
}

// BENCHMARK SYSTEM
void GuiLayer::RunBenchmark(PerformanceTracker &perf, int &objectCount,
                            int &rendererIndex) {
  if (!benchmark)
    return;

  if (!benchmarkStarted) {
    csv.resetContent();

    csv.newRow() << "object_count"
                 << "fps"
                 << "frame_time_ms"
                 << "cpu_time_ms"
                 << "gpu_time_ms"
                 << "renderer"
                 << "draw_calls";

    benchmarkRenderer = 0;
    benchmarkObject = 1;
    currentStep = 0;

    benchmarkTimer = 0.0f;
    measureTimer = 0.0f;

    benchmarkStarted = true;
  }

  float dt = perf.GetDeltaTime();

  rendererIndex = benchmarkRenderer;
  objectCount = benchmarkObject;

  benchmarkTimer += dt;

  // Warmup
  if (benchmarkTimer < warmupTime)
    return;

  // Measurement
  measureTimer += dt;

  accumFPS += perf.GetFPS();
  accumCPU += perf.GetCPUTime();
  accumGPU += perf.GetGPUTime();
  sampleCount++;

  if (measureTimer < measureTime)
    return;

  // Save result
  csv.newRow() << objectCount << (accumFPS / sampleCount) << perf.GetAvgFrame()
               << (accumCPU / sampleCount) << (accumGPU / sampleCount)
               << rendererIndex << drawCalls;

  // Reset
  accumFPS = accumCPU = accumGPU = 0.0f;
  sampleCount = 0;

  measureTimer = 0.0f;
  benchmarkTimer = 0.0f;

  currentStep++;

  if (currentStep >= totalSteps) {
    currentStep = 0;
    benchmarkRenderer++;

    if (benchmarkRenderer > 2) {
      benchmark = false;
      csv.writeToFile("benchmark.csv");
      std::cout << "Benchmark complete!\n";
      return;
    }
  }

  float t = (float)currentStep / (float)(totalSteps - 1);
  benchmarkObject = 1 + t * (50000 - 1);
}

// FRAME END
void GuiLayer::EndFrame(GLFWwindow *window) {
  ImGui::Render();

  int w, h;
  glfwGetFramebufferSize(window, &w, &h);
  glViewport(0, 0, w, h);

  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  ImGuiIO &io = ImGui::GetIO();

  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    GLFWwindow *backup = glfwGetCurrentContext();
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
    glfwMakeContextCurrent(backup);
  }
}

// SHUTDOWN
void GuiLayer::Shutdown() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}
