#pragma once

#include "../RenderPass.h"


// The test pass first will create its own Render Target and Vertex Buffer,
// and then it will render a triangle to the screen.

class Orbit;
class FreeCamera;

struct SunlightData
{
	float lightDirection[4] = { -0.4f, -1.0f, -0.3f, 0.0f };
	float lightColor[4] = { 1.0f, 0.0f, 0.0f, 0.0f };
	float ambientStrength = 0.2f;
	float diffuseStrength = 1.0f;
};

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
	SunlightData sunlightData;
	HBuffer sunlightBuffer;
};
