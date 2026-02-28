#pragma once

#include "IRenderStrategy.h"

class Shader;
class Mesh;
class Camera;
class GLFWwindow; 

class InstancedRenderer : public IRenderStrategy
{
public:
    InstancedRenderer() = default;
    ~InstancedRenderer() override = default;

    void Init() override;
    void Render(int objectCount, Camera& camera, GLFWwindow *window) override;
    void Cleanup() override;

    const char* GetName() const override { return "Instance"; }

private:
    Shader* shader;
    Mesh* mesh;

    unsigned int drawCalls = 0;
};
