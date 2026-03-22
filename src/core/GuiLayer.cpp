#include "core/GuiLayer.h"
#include "tools/PerformanceTracker.h"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>

#include <CSVWriter.h>

static const char *rendererNames[] = {"Naive", "Instanced", "Batch"};

void GuiLayer::Init(GLFWwindow *window) {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();

  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  ImGui::StyleColorsDark();

  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 420");
  benchmarkRenderer = 0;
  currentStep = 0;
  benchmarkObject = 1;
}

void GuiLayer::Render(PerformanceTracker &perf, int &objectCount,
                      int &rendererIndex, bool &vsync) {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  ImGuiWindowFlags window_flags =
      ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking |
      ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
      ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
      ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
      ImGuiWindowFlags_NoBackground;

  ImGuiViewport *viewport = ImGui::GetMainViewport();

  ImGui::SetNextWindowPos(viewport->Pos);
  ImGui::SetNextWindowSize(viewport->Size);
  ImGui::SetNextWindowViewport(viewport->ID);

  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

  ImGui::Begin("MainDockSpace", nullptr, window_flags);
  ImGui::PopStyleVar(2);

  ImGuiID dockspace_id = ImGui::GetID("MainDockSpaceID");
  ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f),
                   ImGuiDockNodeFlags_PassthruCentralNode);

  ImGui::Begin("Benchmark");

  ImGui::SliderInt("Object Count", &objectCount, 1, 50000);

  ImGui::Combo("Renderer", &rendererIndex, rendererNames,
               IM_ARRAYSIZE(rendererNames));

  drawCalls = (rendererIndex == 0) ? objectCount : 1;

  if (ImGui::Checkbox("VSync", &vsync))
    glfwSwapInterval(vsync ? 1 : 0);

  ImGui::Separator();

  // Frame Statistics 

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

  if (ImGui::Button("Run Benchmark")) {
    benchmark = true;
    benchmarkStarted = false;
  }

  if (ImGui::Button("Stop Benchmark")) {
    benchmark = false;
    csv.writeToFile("benchmark.csv");
  }

  // Benchmark System

  if (benchmark && !benchmarkStarted) {
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

    benchmarkTimer = 0.0f;
    measureTimer = 0.0f;

    benchmarkStarted = true;
  }

  if (benchmark) {
    float dt = perf.GetDeltaTime();

    // Force renderer + object count
    rendererIndex = benchmarkRenderer;
    objectCount = benchmarkObject;

    benchmarkTimer += dt;

    // Warmup phase
    if (benchmarkTimer < warmupTime) {
      // do nothing, just stabilize
    } else {
      // Measurement phase
      measureTimer += dt;

      accumFPS += perf.GetFPS();
      accumCPU += perf.GetCPUTime();
      accumGPU += perf.GetGPUTime();
      sampleCount++;

      if (measureTimer >= measureTime) {
        // Save averaged result
        csv.newRow() << objectCount << (accumFPS / sampleCount)
                     << perf.GetAvgFrame() << (accumCPU / sampleCount)
                     << (accumGPU / sampleCount) << rendererIndex << drawCalls;

        // Reset accumulators
        accumFPS = accumCPU = accumGPU = 0.0f;
        sampleCount = 0;

        measureTimer = 0.0f;
        benchmarkTimer = 0.0f;

        currentStep++;

        if (currentStep >= totalSteps) {
          // next renderer
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
    }
  }

  ImGui::End();
  ImGui::End();

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GuiLayer::Shutdown() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}
