#include "Engine.h"
#include "debugapi.h"
#include "DeviceContext.h"
#include "WindowContext.h"
#include "RenderContext.h"
#include "RenderGraph.h"
#include "Settings.h"
#include "Observer.h"
#include "input/RawInput.h"
#include "../externals/AssetSuite/inc/AssetSuite.h"

#include <filesystem>

#define FRAME_COUNT 2

// Global Entities
Settings settings;
WindowContext windowContext;
DeviceContext deviceContext;
RenderContext renderContext;
RenderGraph renderGraph;
Subject<WinMessageEvent> winMessageSubject;
RawInput rawInput;

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
	rawInput.Initialize();
	winMessageSubject.Subscribe(&rawInput);
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
	//currentPath.append("teapot.obj");
	//currentPath.append("cube.obj");
	//currentPath.append("temple.obj");
	currentPath.append("chess.obj");

	AssetSuite::Manager assetManager;
	assetManager.MeshLoadAndDecode(currentPath.string().c_str(), AssetSuite::MeshDecoders::WAVEFRONT);

	std::vector<FLOAT> meshOutput;
	AssetSuite::MeshDescriptor meshDescriptor;

	//std::vector<std::string> meshNames = {
	//	"Building_Mesh",
	//	"RoofBase_Mesh",
	//	"ColumnOne_Mesh",
	//	"ColumnTwo_Mesh",
	//	"ColumnThree_Mesh",
	//	"ColumnFour_Mesh",
	//	"Roof_Mesh",
	//	"RoofEdge_Mesh"
	//};

	std::vector<std::string> meshNames = {
		"Chess_Board_Mesh",
		"Pawn_3_Dark_Mesh",
		"Bishop_Dark_Mesh",
		"Tower_2_Dark_Mesh",
		"Queen_Dark_Mesh",
		"King_Dark_Mesh",
		"Knight_2_Dark_Mesh",
		"Knight_Dark_Mesh",
		"Tower_Dark_Mesh",
		"Bishop_2_Dark_Mesh",
		"Pawn_2_Dark_Mesh",
		"Pawn_4_Dark_Mesh",
		"Pawn_5_Dark_Mesh",
		"Pawn_6_Dark_Mesh",
		"Pawn_7_Dark_Mesh",
		"Pawn_8_Dark_Mesh",
		"Pawn_Dark_Mesh",
		"Pawn_3_Light_Mesh",
		"Bishop_Light_Mesh",
		"Tower_2_Light_Mesh",
		"Queen_Light_Mesh",
		"King_Light_Mesh",
		"Knight_2_Light_Mesh",
		"Knight_Light_Mesh",
		"Tower_Light_Mesh",
		"Bishop_2_Light_Mesh",
		"Pawn_2_Light_Mesh",
		"Pawn_4_Light_Mesh",
		"Pawn_5_Light_Mesh",
		"Pawn_6_Light_Mesh",
		"Pawn_7_Light_Mesh",
		"Pawn_8_Light_Mesh",
		"Pawn_Light_Mesh"
	};

	for (const auto& meshName : meshNames)
	{
		auto errorCode = assetManager.MeshGet(meshName.c_str(), AssetSuite::MeshOutputFormat::POSITION, meshOutput, meshDescriptor);
		if (errorCode != AssetSuite::ErrorCode::OK)
		{
			OutputDebugString(L"Failed to load mesh: ");
			OutputDebugStringA(meshName.c_str());
			OutputDebugString(L"\n");
			continue;
		}
		auto numOfTriangles = meshDescriptor.numOfVertices;
		CHAR tempName[64];
		snprintf(tempName, sizeof(tempName), "POSITION_%s", meshName.c_str());
		auto vbIndexPositionAndColor = renderContext.CreateVertexBuffer(numOfTriangles * 3, 4, meshOutput.data(), tempName);
		auto vbIndexColor = renderContext.GenerateColors(meshOutput.data(), meshOutput.size(), numOfTriangles, meshName.c_str());
		renderContext.CreateMesh(vbIndexPositionAndColor, vbIndexColor,meshName.c_str());
	}
}

void Engine::Tick()
{
	if (rawInput.IsKeyDown(VK_ESCAPE))
	{
		OutputDebugString(L"Engine::Tick() - Escape key pressed\n");
		exit(0);
	}
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
		winMessageSubject.RunPostFrame();
	}
}
