#pragma once

#include "../RenderPass.h"

// Type: Compute
// Clears the internal seed texture to the INVALID value, to distinguish the seeds from non-seed
// pixels.

class Orbit;

class HighlightClearPass : public RenderPass
{
public:
    HighlightClearPass();
    ~HighlightClearPass();

    void ConfigurePipelineState() override;
    void PostAssetLoad() override;
    void Initialize() override;
    void Update() override;
    void Execute() override;
    void PostSubmit() override;
    void Allocate(DeviceContext* deviceContext) override;
private:
	HTexture outputTexture;
};
