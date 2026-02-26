#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>

#include "core/Camera.h"
#include "renderers/BatchRenderer.h"
#include "renderers/IRenderStrategy.h"
#include "renderers/InstancedRenderer.h"
#include "renderers/NaiveRenderer.h"

// --------------------------------
// Settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// --------------------------------
// Camera
Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));

float deltaTime = 0.0f;
float lastFrame = 0.0f;

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool cursorEnabled = false;

// --------------------------------
// Forward declarations
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

// --------------------------------
// Main
int main() {
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
  glfwWindowHint(GLFW_OPENGL_CORE_PROFILE, GL_TRUE);

  GLFWwindow *window =
      glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "GL-Bench", NULL, NULL);
  if (!window) {
    std::cout << "Failed to create window\n";
    return -1;
  }

  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to init GLAD\n";
    return -1;
  }

  glEnable(GL_DEPTH_TEST);

  // --------------------------------
  // Setup ImGui
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGui::StyleColorsDark();

  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 420 core");

  // --------------------------------
  // Renderer Setup
  int currentRendererIndex = 0;
  const char *rendererNames[] = {"Naive", "Batch", "Instanced"};

  auto createRenderer = [&](int index) -> IRenderStrategy * {
    if (index == 0)
      return new NaiveRenderer();
    if (index == 1)
      return new BatchRenderer();
    if (index == 2)
      return new InstancedRenderer();
    return nullptr;
  };

  IRenderStrategy *renderer = createRenderer(currentRendererIndex);
  renderer->Init();

  int objectCount = 100;
  bool vsync = false;

  // --------------------------------
  // Render Loop
  while (!glfwWindowShouldClose(window)) {
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    processInput(window);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render scene
    renderer->Render(objectCount, camera, window);

    // ImGui Frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Benchmark");

    ImGui::SliderInt("Object Count", &objectCount, 1, 50000);

    if (ImGui::Combo("Renderer", &currentRendererIndex, rendererNames,
                     IM_ARRAYSIZE(rendererNames))) {
      renderer->Cleanup();
      delete renderer;
      renderer = createRenderer(currentRendererIndex);
      renderer->Init();
    }

    if (ImGui::Checkbox("VSync", &vsync)) {
      glfwSwapInterval(vsync ? 1 : 0);
    }

    ImGui::Separator();
    ImGui::Text("FPS: %.1f", 1.0f / deltaTime);
    ImGui::Text("Frame Time: %.3f ms", deltaTime * 1000.0f);

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  renderer->Cleanup();
  delete renderer;

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwTerminate();
  return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this
void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  static bool keyHeld = false;
  // toggle with F or G
  if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS ||
      glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
    if (!keyHeld) {
      cursorEnabled = !cursorEnabled;
      glfwSetInputMode(window, GLFW_CURSOR,
                       cursorEnabled ? GLFW_CURSOR_NORMAL
                                     : GLFW_CURSOR_DISABLED);
      keyHeld = true;
    }
  } else {
    keyHeld = false;
  }
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    camera.ProcessKeyboard(FORWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    camera.ProcessKeyboard(BACKWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    camera.ProcessKeyboard(LEFT, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
  if (cursorEnabled) {
    firstMouse = true;
    return;
  }
  if (firstMouse) {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }

  float xoffset = xpos - lastX;
  float yoffset = lastY - ypos;

  lastX = xpos;
  lastY = ypos;

  camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
  camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
