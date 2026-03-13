#include "core/Callbacks.h"
#include "tools/EngineConfig.h"

// Framebuffer

void Callbacks::framebuffer_size_callback(GLFWwindow *window, int width,
                                          int height) {
  glViewport(0, 0, width, height);
  EngineConfig::WindowWidth = width;
  EngineConfig::WindowHeight = height;
}

// Mouse

void Callbacks::mouse_callback(GLFWwindow *window, double xpos, double ypos) {
  Callbacks *cb = static_cast<Callbacks *>(glfwGetWindowUserPointer(window));
  if (!cb || !cb->camera)
    return;

  if (cb->cursorEnabled) {
    cb->firstMouse = true;
    return;
  }

  if (cb->firstMouse) {
    cb->lastX = static_cast<float>(xpos);
    cb->lastY = static_cast<float>(ypos);
    cb->firstMouse = false;
  }

  float xoffset = static_cast<float>(xpos) - cb->lastX;
  float yoffset = cb->lastY - static_cast<float>(ypos);

  cb->lastX = static_cast<float>(xpos);
  cb->lastY = static_cast<float>(ypos);

  cb->camera->ProcessMouseMovement(xoffset, yoffset);
}

// Scroll

void Callbacks::scroll_callback(GLFWwindow *window, double xoffset,
                                double yoffset) {
  Callbacks *cb = static_cast<Callbacks *>(glfwGetWindowUserPointer(window));
  if (!cb || !cb->camera)
    return;

  cb->camera->ProcessMouseScroll(static_cast<float>(yoffset));
}

// Keyboard

void Callbacks::processInput(GLFWwindow *window, float deltaTime) {
  if (!camera)
    return;

  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  static bool keyHeld = false;

  if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
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
    camera->ProcessKeyboard(FORWARD, deltaTime);

  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    camera->ProcessKeyboard(BACKWARD, deltaTime);

  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    camera->ProcessKeyboard(LEFT, deltaTime);

  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    camera->ProcessKeyboard(RIGHT, deltaTime);
}
