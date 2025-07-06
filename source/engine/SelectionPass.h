#include "RenderPass.h"

class SelectionPass : public RenderPass
{
public:
	SelectionPass();
	void ConfigurePipelineState() override;
	void Initialize() override;
	void Update() override;
	void Execute() override;
	void Allocate(DeviceContext* deviceContext) override;
};