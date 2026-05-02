#pragma once

#include "../RenderPass.h"


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
private:
	FreeCamera* freeCamera;
	BOOL isPerspectiveCamera = TRUE;
	HBuffer sunlightBuffer;
};
