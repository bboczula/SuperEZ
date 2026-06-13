#include "../RenderPass.h"

class HighlightPass : public RenderPass
{
public:
	HighlightPass();
	void ConfigurePipelineState() override;
	void PostAssetLoad() override;
	void Initialize() override;
	void Update() override;
	void Execute() override;
	void PostSubmit() override;
	void Allocate(DeviceContext* deviceContext) override;
	void RegisterSettings(RenderPassSettings& registry) override;
private:
	HTexture outputTexture;
};
