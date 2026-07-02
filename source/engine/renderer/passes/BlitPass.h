#pragma once

#include "../RenderPass.h"

#include <vector>

// This pass will copy whichever texture requested to the back buffer.
// A debug source can be picked in the Render Pass Settings window; it is
// decoded and letterbox-scaled into an intermediate texture by a compute
// shader (debug_blit.hlsl), which is then copied to the back buffer.

class BlitPass : public RenderPass
{
public:
	BlitPass();
	~BlitPass();
	void ConfigurePipelineState() override;
	void PostAssetLoad() override;
	void Initialize() override;
	void Update() override;
	void Execute() override;
	void PostSubmit() override;
	void Allocate(DeviceContext* deviceContext) override;
	void RegisterSettings(RenderPassSettings& settings) override;
private:
	// Must match the MODE_* defines in debug_blit.hlsl.
	enum VisualizationMode : int
	{
		Color = 0,
		UintId = 1,
		Depth = 2,
		// R32_FLOAT render targets carry raw uint bits (see RenderTargetFormat::R32_UINT mapping)
		UintBitcast = 3
	};

	struct DebugSource
	{
		HTexture texture;
		int mode = VisualizationMode::Color;
	};

	HTexture GetDefaultSourceTexture() const;
private:
	// Intermediate back-buffer-sized RGBA8 target for the debug visualization.
	HTexture debugTexture;
	// 0 = Default (normal frame output), i > 0 = debugSources[i - 1].
	int debugSourceIndex = 0;
	std::vector<DebugSource> debugSources;
	// Backing storage for the combo labels; RenderPassSetting keeps raw pointers,
	// so this must live as long as the pass does.
	std::vector<const char*> debugSourceItems;
	// Info block about the current source, shown below the combo. Rewritten
	// every frame in Update(); registered with the settings UI by pointer.
	char sourceInfo[256] = "";
};
