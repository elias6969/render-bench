#pragma once

#include "IRenderStrategy.h"

class Shader;
class Mesh;

class BatchRenderer : public IRenderStrategy
{
public:
    BatchRenderer() = default;
    ~BatchRenderer() override = default;

    void Init() override;
    void Render(int objectCount, Camera& camera, GLFWwindow *window) override;
    void Cleanup() override;

    const char* GetName() const override { return "Batch"; }

private:
    Shader* shader;
    Mesh* mesh;

    unsigned int drawCalls = 0;
};
