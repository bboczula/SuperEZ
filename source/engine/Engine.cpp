#include "Engine.h"
#include "debugapi.h"
#include "DeviceContext.h"
#include "WindowContext.h"
#include "RenderContext.h"
#include "RenderGraph.h"
#include "Settings.h"
#include "../externals/AssetSuite/inc/AssetSuite.h"

#include <filesystem>

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
	renderGraph.Initialize();
}

void Engine::CreateRenderResources()
{
	// Here we can make engine speciffic allocations
}

void Engine::LoadAssets()
{
	std::filesystem::path currentPath = std::filesystem::current_path();
	//currentPath.append("monkey.obj");
	currentPath.append("teapot.obj");
	//currentPath.append("cube.obj");

	AssetSuite::Manager assetManager;
	assetManager.MeshLoadAndDecode(currentPath.string().c_str(), AssetSuite::MeshDecoders::WAVEFRONT);

	std::vector<FLOAT> meshOutput;
	AssetSuite::MeshDescriptor meshDescriptor;
	//auto errorCode = assetManager.MeshGet("Suzanne_Mesh", AssetSuite::MeshOutputFormat::POSITION, meshOutput, meshDescriptor);
	auto errorCode = assetManager.MeshGet("teapot_Mesh", AssetSuite::MeshOutputFormat::POSITION, meshOutput, meshDescriptor);
	//auto errorCode = assetManager.MeshGet("Cube_Mesh", AssetSuite::MeshOutputFormat::POSITION, meshOutput, meshDescriptor);

	const auto numOfTriangles = meshDescriptor.numOfVertices;

	renderContext.CreateMesh(meshOutput.data(), meshOutput.size(), numOfTriangles);
	//deviceContext.Flush();
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

	LoadAssets();

	MSG msg{ 0 };
	while (1)
	{
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				OutputDebugString(L"Engine::Run() - WM_QUIT received\n");
				exit(0);
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		Tick();
	}
}
