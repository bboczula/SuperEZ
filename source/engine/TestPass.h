#pragma once

#include "RenderPass.h"


// The test pass first will create its own Render Target and Vertex Buffer,
// and then it will render a triangle to the screen.

class PerspectiveCamera;
class OrthographicCamera;
class Arcball;
class Camera;

class TestPass : public RenderPass
{
public:
	TestPass();
	~TestPass();
	void ConfigurePipelineState() override;
	void Prepare() override;
	void Update() override;
	void Execute() override;
	void Allocate(DeviceContext* deviceContext) override;
private:
	PerspectiveCamera* perspectiveCamera;
	OrthographicCamera* orthoCamera;
	Arcball* arcballCamera;
};