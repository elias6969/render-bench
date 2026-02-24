#ifndef ENGINE_CONFIG_H
#define ENGINE_CONFIG_H

#include <string>

struct GLFWwindow; // forward declaration

class EngineConfig {
public:
    // Asset directories
    static std::string TextureDirectory;
    static std::string ShaderDirectory;
    static std::string CubemapDirectory;
    static std::string CubemapTestDirectory;
    static std::string FontDirectory;
    static std::string ModelDirectory;

    // Window / framebuffer state
    static unsigned int WindowWidth;
    static unsigned int WindowHeight;
};

// Updates framebuffer + engine window state
void UpdateFramebufferSize(GLFWwindow* window, int& width, int& height);

#endif // ENGINE_CONFIG_H
