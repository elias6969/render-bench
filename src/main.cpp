#include "core/Application.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

int main()
{
    Application app;
    app.Run();
    return 0;
}
