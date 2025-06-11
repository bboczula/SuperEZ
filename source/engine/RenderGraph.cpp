#include "RenderGraph.h"
#include "RenderPass.h"
#include "TestPass.h"
#include "ImGuiPass.h"
#include "BlitPass.h"

RenderGraph::RenderGraph()
{
	// Here we could define our render passes
	RenderPass* testPass = new TestPass();
	renderPasses.push_back(testPass);

	RenderPass* imguiPass = new ImGuiPass();
	renderPasses.push_back(imguiPass);

	RenderPass* blitPass = new BlitPass();
	renderPasses.push_back(blitPass);

	// Here we could define the dependencies between the render passes

	// Maybe we could put the final blit here too
}

void RenderGraph::Execute()
{
	for (RenderPass* renderPass : renderPasses)
	{
		renderPass->Update();
		renderPass->PreExecute();
		renderPass->Execute();
		renderPass->PostExecute();
	}
}

void RenderGraph::Initialize()
{
	for (RenderPass* renderPass : renderPasses)
	{
		renderPass->ConfigurePipelineState();
		renderPass->AutomaticInitialize();
		renderPass->Initialize();
	}
}
