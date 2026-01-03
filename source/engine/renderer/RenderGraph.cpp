#include "RenderGraph.h"
#include "RenderPass.h"
#include "passes/TestPass.h"
#include "passes/ImGuiPass.h"
#include "passes/BlitPass.h"
#include "passes/SelectionPass.h"
#include "passes/GrayscalePass.h"

RenderGraph::RenderGraph()
{
	RenderPass* selectionPass = new SelectionPass();
	renderPasses.push_back(selectionPass);

	RenderPass* testPass = new TestPass();
	renderPasses.push_back(testPass);

	RenderPass* grayscalePass = new GrayscalePass();
	renderPasses.push_back(grayscalePass);

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
