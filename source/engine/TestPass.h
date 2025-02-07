#pragma once

#include "RenderPass.h"
#include "camera/PerspectiveCamera.h"
#include "camera/Arcball.h"

// The test pass first will create its own Render Target and Vertex Buffer,
// and then it will render a triangle to the screen.

class TestPass : public RenderPass
{
public:
	TestPass();
	~TestPass();
	void Prepare() override;
	void Update() override;
	void Execute() override;
	void Allocate(DeviceContext* deviceContext) override;
private:
	PerspectiveCamera* camera;
	Arcball* arcballCamera;
};