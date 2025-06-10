#pragma once

#include "RenderPass.h"

class ImGuiPass : public RenderPass
{
public:
	ImGuiPass();
	void ConfigurePipelineState() override;
	void Initialize() override;
	void Update() override;
	void Execute() override;
	void Allocate(DeviceContext* deviceContext) override;
};