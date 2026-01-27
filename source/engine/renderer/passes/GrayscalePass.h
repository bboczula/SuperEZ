#pragma once

#include "../RenderPass.h"

// A render pass that applies a grayscale effect to the input render target.

class GrayscalePass : public RenderPass
{
public:
	GrayscalePass();
	~GrayscalePass();
	void ConfigurePipelineState() override;
	void PostAssetLoad() override;
	void Initialize() override;
	void Update() override;
	void Execute() override;
	void PostSubmit() override;
	void Allocate(DeviceContext* deviceContext) override;
private:
	HTexture outputTexture;
};