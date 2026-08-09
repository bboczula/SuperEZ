#pragma once

#include "../RenderPass.h"
#include "../RenderContext.h"


// The forward pass renders the scene from the active camera.

class Orbit;
class FreeCamera;

class ForwardPass : public RenderPass
{
public:
	ForwardPass();
	void SetOrthographicProperties(const float aspectRatio);
	void PostAssetLoad() override;
	~ForwardPass();
	void ConfigurePipelineState() override;
	void Initialize() override;
	void Update() override;
	void Execute() override;
	void PostSubmit() override;
	void Allocate(DeviceContext* deviceContext) override;
	void RegisterSettings(RenderPassSettings& registry) override;
private:
	FreeCamera* freeCamera;
	BOOL isPerspectiveCamera = TRUE;
	HBuffer sunlightBuffer;
	HBuffer sunlightViewProjectionBuffer;
	HBuffer debugSettingsBuffer;
	DebugSettings debugSettings;
	// Debug comparison only. sRGB mip generation remains enabled in both modes.
	bool useLinearLighting = true;
	bool visualizeSelectedMip = false;
};
