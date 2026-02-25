#pragma once

#include "IRenderStrategy.h"

class Shader;
class Mesh;

class NaiveRenderer : public IRenderStrategy
{
public:
    NaiveRenderer();
    ~NaiveRenderer() override;

    void Init() override;
    void Render(int objectCount) override;
    void Cleanup() override;

    const char* GetName() const override { return "Naive"; }

private:
    Shader* shader;
    Mesh* mesh;

    unsigned int drawCalls = 0;
};
