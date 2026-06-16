#include <vector>
#include "RenderPassSettings.h"

class RenderPass;

class RenderGraph
{
public:
	RenderGraph();
	void Execute();
	void Initialize();
	void PostAssetLoad();
private:
	std::vector<RenderPass*> renderPasses;
	RenderPassSettings settings;
};
