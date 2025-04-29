#pragma once

#include "RenderPass.h"


// The test pass first will create its own Render Target and Vertex Buffer,
// and then it will render a triangle to the screen.

class Camera;
class Camera;
class Orbit;
class Camera;

class TestPass : public RenderPass
{
public:
	TestPass();
	void SetOrthographicProperties(const float aspectRatio);
	~TestPass();
	void ConfigurePipelineState() override;
	void Prepare() override;
	void Update() override;
	void Execute() override;
	void Allocate(DeviceContext* deviceContext) override;
private:
	Camera* camera;
	Orbit* arcballCamera;
	BOOL isPerspectiveCamera = TRUE;
};