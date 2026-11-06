#pragma once

#include "../RenderPass.h"

// Type: Graphics
// Render Pass used for rendering the User Interface, including labels, initially using the bitmap
// font rendering.

class Orbit;
struct EngineAssets;

class UserInterfacePass : public RenderPass
{
public:
    explicit UserInterfacePass(const EngineAssets& engineAssets);
    ~UserInterfacePass();

    void ConfigurePipelineState() override;
    void PostAssetLoad() override;
    void Initialize() override;
    void Update() override;
    void Execute() override;
	void PostSubmit() override;
	void Allocate(DeviceContext* deviceContext) override;
private:
	const EngineAssets& engineAssets;
	HMesh letterMesh;
};
