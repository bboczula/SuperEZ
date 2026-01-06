#include "../RenderPass.h"

class HighlightPass : public RenderPass
{
public:
	HighlightPass();
	void ConfigurePipelineState() override;
	void Initialize() override;
	void Update() override;
	void Execute() override;
	void Allocate(DeviceContext* deviceContext) override;
private:
	HTexture outputTexture;
};