#pragma once

#include "IRenderStrategy.h"

class Shader;
class Mesh;

class InstancedRenderer : public IRenderStrategy
{
public:
    InstancedRenderer();
    ~InstancedRenderer() override;

    void Init() override;
    void Render(int objectCount) override;
    void Cleanup() override;

    const char* GetName() const override { return "Instance"; }

private:
    Shader* shader;
    Mesh* mesh;

    unsigned int drawCalls = 0;
};
