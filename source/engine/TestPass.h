#pragma once

#include "RenderPass.h"

// The test pass first will create its own Render Target and Vertex Buffer,
// and then it will render a triangle to the screen.

class TestPass : public RenderPass
{
public:
	TestPass();
	~TestPass();
	void Prepare() override;
	void Execute() override;
	void Allocate(DeviceContext* deviceContext) override;
private:
};