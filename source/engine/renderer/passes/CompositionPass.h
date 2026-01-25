#pragma once

#include "../RenderPass.h"

// Type: Compute
// Combines several output (like highlight or debug) with the final color texture

class Orbit;

class CompositionPass : public RenderPass
{
public:
    CompositionPass();
    ~CompositionPass();

    void ConfigurePipelineState() override;
    void PostAssetLoad() override;
    void Initialize() override;
    void Update() override;
    void Execute() override;
    void Allocate(DeviceContext* deviceContext) override;
private:
	HTexture outputTexture;
};
