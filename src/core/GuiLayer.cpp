#include "core/GuiLayer.h"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <cmath>
#include <imgui.h>

static const char *rendererNames[] = {"Naive", "Instanced", "Batch"};

void GuiLayer::Init(GLFWwindow *window) {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui::StyleColorsDark();

  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 420");
}

void GuiLayer::Render(int &objectCount, int &rendererIndex, bool &vsync) {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  ImGui::Begin("Benchmark");

  ImGui::SliderInt("Object Count", &objectCount, 1, 50000);

  ImGui::Combo("Renderer", &rendererIndex, rendererNames,
               IM_ARRAYSIZE(rendererNames));

  if (ImGui::Checkbox("VSync", &vsync))
    glfwSwapInterval(vsync ? 1 : 0);

  ImGui::End();
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void GuiLayer::Shutdown() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}
