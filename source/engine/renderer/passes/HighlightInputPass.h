#pragma once

#include "../RenderPass.h"

// Type: Graphics
// Creates an input for the actual Highlight shader. It will take a set of objects and render them
// as seeds and will set all other pixels as some value that represents the uninitialized state.

class Orbit;

class HighlightInputPass : public RenderPass
{
public:
    HighlightInputPass();
    ~HighlightInputPass();

    void ConfigurePipelineState() override;
    void PostAssetLoad() override;
    void Initialize() override;
    void Update() override;
    void Execute() override;
    void Allocate(DeviceContext* deviceContext) override;
};
