#pragma once

#include "../RenderPass.h"

// Type: Graphics
// Responsible for generating the Shadow Map for the scene

class Orbit;

class ShadowMapPass : public RenderPass
{
public:
    ShadowMapPass();
    ~ShadowMapPass();

    void ConfigurePipelineState() override;
    void PostAssetLoad() override;
    void Initialize() override;
    void Update() override;
    void Execute() override;
    void PostSubmit() override;
    void Allocate(DeviceContext* deviceContext) override;
private:
    static constexpr UINT ShadowMapSize = 2048;
    HTexture shadowMapTexture;
    HBuffer lightViewProjectionBuffer;
};
