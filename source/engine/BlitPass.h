#pragma once

#include "RenderPass.h"

// This pass will copy whichever texture requested to the back buffer

class BlitPass : public RenderPass
{
public:
	BlitPass();
	~BlitPass();
	void Prepare() override;
	void Execute() override;
	void Allocate(DeviceContext* deviceContext) override;
private:
};