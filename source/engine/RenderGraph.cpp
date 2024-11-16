#include "RenderGraph.h"
#include "RenderPass.h"
#include "TestPass.h"

RenderGraph::RenderGraph()
{
	// Here we could define our render passes
	RenderPass* testPass = new TestPass();
	renderPasses.push_back(testPass);

	// Here we could define the dependencies between the render passes

	// Maybe we could put the final blit here too
}

void RenderGraph::Execute()
{
	for (RenderPass* renderPass : renderPasses)
	{
		renderPass->Prepare();
		renderPass->Execute();
	}
}