#include "Engine.h"
#include "debugapi.h"
#include "DeviceContext.h"
#include "WindowContext.h"
#include "RenderContext.h"
#include "RenderGraph.h"
#include "Settings.h"

#define FRAME_COUNT 2

// Global Entities
Settings settings;
WindowContext windowContext;
DeviceContext deviceContext;
RenderContext renderContext;
RenderGraph renderGraph;

Engine::Engine()
{
	OutputDebugString(L"Engine Constructor\n");
}

Engine::~Engine()
{
	OutputDebugString(L"Engine Destructor\n");
}

void Engine::Initialize()
{
	OutputDebugString(L"Engine::Initialize()\n");
	renderContext.CreateDescriptorHeap(&deviceContext);
	renderContext.CreateRenderTargetFromBackBuffer(&deviceContext);
	renderContext.CreateCommandBuffer(&deviceContext);
}

void Engine::CreateRenderResources()
{
	renderContext.CreateRootSignature(&deviceContext);
	renderContext.CreateShaders(&deviceContext);
	renderContext.CreatePipelineState(&deviceContext);
	renderContext.CreateViewportAndScissorRect(&deviceContext);
	renderContext.CreateVertexBuffer(&deviceContext);
	deviceContext.Flush();
}

void Engine::Tick()
{
	renderGraph.Execute();
	renderContext.PopulateCommandList(&deviceContext);
	renderContext.ExecuteCommandList(&deviceContext);
	deviceContext.Flush();
	deviceContext.Present();
	deviceContext.Flush();
}

void Engine::Run()
{
	MSG msg{ 0 };
	while (1)
	{
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				OutputDebugString(L"Sapphire::WindowApplication::Run() - WM_QUIT received\n");
				exit(0);
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		Tick();
	}
}
