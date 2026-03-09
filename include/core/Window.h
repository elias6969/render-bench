#pragma once
#include <GLFW/glfw3.h>

class Window
{
public:
    Window(int width, int height, const char* title);
    ~Window();

    void PollEvents();
    void SwapBuffers();
    bool ShouldClose() const;

    GLFWwindow* GetNative() const { return m_Window; }

private:
    GLFWwindow* m_Window;
};
