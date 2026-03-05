#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <numeric>
#include <vector>

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui.h>

#include "core/Camera.h"
#include "core/Application.h"
#include "core/Callbacks.h"

#include "renderers/BatchRenderer.h"
#include "renderers/IRenderStrategy.h"
#include "renderers/InstancedRenderer.h"
#include "renderers/NaiveRenderer.h"

int main()
{
    Application app;
    app.Run();
    return 0;
}
