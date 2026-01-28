#pragma once

#include "../RenderPass.h"

// This pass will copy whichever texture requested to the back buffer

class BlitPass : public RenderPass
{
public:
	BlitPass();
	~BlitPass();
	void ConfigurePipelineState() override;
	void PostAssetLoad() override;
	void Initialize() override;
	void Execute() override;
	void PostSubmit() override;
	void Allocate(DeviceContext* deviceContext) override;
private:
};