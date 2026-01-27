#pragma once

#include "../RenderPass.h"


// The test pass first will create its own Render Target and Vertex Buffer,
// and then it will render a triangle to the screen.

class Orbit;
class FreeCamera;

class TestPass : public RenderPass
{
public:
	TestPass();
	void SetOrthographicProperties(const float aspectRatio);
	void PostAssetLoad() override;
	~TestPass();
	void ConfigurePipelineState() override;
	void Initialize() override;
	void Update() override;
	void Execute() override;
	void PostSubmit() override;
	void Allocate(DeviceContext* deviceContext) override;
private:
	FreeCamera* freeCamera;
	BOOL isPerspectiveCamera = TRUE;
};