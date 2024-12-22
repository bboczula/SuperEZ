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
	renderContext.CreateTexture(&deviceContext);
	renderGraph.Initialize();
}

void Engine::CreateRenderResources()
{
	// Here we can make engine speciffic allocations
}

void Engine::Tick()
{
	renderGraph.Execute();
	deviceContext.Flush();
	deviceContext.Present();
	deviceContext.Flush();
}

void Engine::Run()
{
	Initialize();

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
