#include <vector>

class RenderPass;

class RenderGraph
{
public:
	RenderGraph();
	void Execute();
	void Initialize();
private:
	std::vector<RenderPass*> renderPasses;
};