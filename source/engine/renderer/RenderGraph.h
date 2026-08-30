#include <vector>
#include "RenderPassSettings.h"

class RenderPass;
struct EngineAssets;

class RenderGraph
{
public:
	explicit RenderGraph(const EngineAssets& engineAssets);
	void Execute();
	void Initialize();
	void PostAssetLoad();
private:
	const EngineAssets& engineAssets;
	std::vector<RenderPass*> renderPasses;
	RenderPassSettings settings;
};
