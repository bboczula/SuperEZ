#include <vector>

class RenderPass;

class RenderGraph
{
public:
	RenderGraph();
	void Execute();
private:
	std::vector<RenderPass*> renderPasses;
};