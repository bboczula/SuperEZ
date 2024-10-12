#include <vector>

class RenderPass;

class RenderGraph
{
public:
	RenderGraph();
private:
	std::vector<RenderPass*> renderPasses;
};