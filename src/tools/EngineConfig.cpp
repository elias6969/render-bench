#include "tools/EngineConfig.h"
#include "tools/filemanager.h"

#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Safe, lazy-initialized VFS
static VirtualFileSystem& GetVirtualFileSystem() {
    static VirtualFileSystem vfs("../assets");
    return vfs;
}

// Asset paths
std::string EngineConfig::TextureDirectory =
    GetVirtualFileSystem().getFullPath("Textures/");

std::string EngineConfig::ShaderDirectory =
    GetVirtualFileSystem().getFullPath("Shaders/");

std::string EngineConfig::CubemapDirectory =
    GetVirtualFileSystem().getFullPath("Cubemap/");

std::string EngineConfig::CubemapTestDirectory =
    GetVirtualFileSystem().getFullPath("Cubemap/test/");

std::string EngineConfig::FontDirectory =
    GetVirtualFileSystem().getFullPath("Fonts/");

std::string EngineConfig::ModelDirectory =
    GetVirtualFileSystem().getFullPath("Models/");

// Default window size
unsigned int EngineConfig::WindowWidth  = 800;
unsigned int EngineConfig::WindowHeight = 600;

void UpdateFramebufferSize(GLFWwindow* window, int& width, int& height) {
    glfwGetFramebufferSize(window, &width, &height);

    EngineConfig::WindowWidth  = static_cast<unsigned int>(width);
    EngineConfig::WindowHeight = static_cast<unsigned int>(height);
}
