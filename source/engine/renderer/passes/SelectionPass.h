#include "../RenderPass.h"

class SelectionPass : public RenderPass
{
public:
	SelectionPass();
	void ConfigurePipelineState() override;
	void PostAssetLoad() override;
	void Initialize() override;
	void Update() override;
	void Execute() override;
	void Allocate(DeviceContext* deviceContext) override;
private:
	HBuffer readbackBuffer;
	HANDLE readbackEvent = nullptr;
	UINT64 fenceValue = 0;
	UINT64 lastRequestedFenceValue = 0;
	bool waitingForReadback = false;
	ID3D12Fence* readbackFence = nullptr;
	bool skipFrame = true; // Skip frame if no selection is made
};