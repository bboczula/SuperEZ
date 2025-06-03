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
	renderContext.CreateDefaultSamplers();
	renderContext.CreateRenderTargetFromBackBuffer(&deviceContext);
	renderGraph.Initialize();
}

void Engine::CreateRenderResources()
{
	// Here we can make engine speciffic allocations
}

void Engine::LoadAssets(GameObjects gameObjects, std::filesystem::path currentPath)
{
	AssetSuite::Manager assetManager;
	assetManager.MeshLoadAndDecode(currentPath.string().c_str(), AssetSuite::MeshDecoders::WAVEFRONT);

	std::vector<FLOAT> meshOutput;
	AssetSuite::MeshDescriptor meshDescriptor{};
	std::vector<BYTE> imageOutput;
	AssetSuite::ImageDescriptor imageDescriptor{};

	auto LogMeshError = [](const std::string& meshName)
		{
			OutputDebugStringW(L"Failed to load mesh: ");
			OutputDebugStringA(meshName.c_str());
			OutputDebugStringW(L"\n");
		};

	for (const auto& [meshName, textureName] : gameObjects)
	{
		auto TryGetMesh = [&](AssetSuite::MeshOutputFormat format) -> bool
		{
			auto errorCode = assetManager.MeshGet(meshName.c_str(), format, meshOutput, meshDescriptor);
			if (errorCode != AssetSuite::ErrorCode::OK)
			{
				LogMeshError(meshName);
				return false;
			}
			return true;
		};

		auto CreateVB = [&](std::string_view label, int components) -> HVertexBuffer
		{
			std::string name = std::string(label) + "_" + meshName;
			return renderContext.CreateVertexBuffer(meshDescriptor.numOfVertices * 3, components, meshOutput.data(), name.c_str());
		};

		auto CreateTexture = [&](const AssetSuite::ImageDescriptor& desc, std::vector<BYTE>& data)
		{
			std::string name = "TEX_" + meshName;
			renderContext.CreateTexture(desc.width, desc.height, data.data(), name.c_str());
		};

		if (!TryGetMesh(AssetSuite::MeshOutputFormat::POSITION))
			continue;

		auto vbPosition = CreateVB("POSITION", 4);
		auto vbColor = renderContext.GenerateColors(meshOutput.data(), meshOutput.size(), meshDescriptor.numOfVertices, meshName.c_str());

		if (!TryGetMesh(AssetSuite::MeshOutputFormat::TEXCOORD))
			continue;

		auto vbTexcoord = CreateVB("TEXCOORD", 2);
		renderContext.CreateMesh(vbPosition, vbColor, vbTexcoord, meshName.c_str());

		auto texturePath = std::filesystem::current_path() / textureName;
		assetManager.ImageLoadAndDecode(texturePath.string().c_str());
		assetManager.ImageGet(AssetSuite::OutputFormat::RGB8, imageOutput, imageDescriptor);

		CreateTexture(imageDescriptor, imageOutput);
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

	std::filesystem::path currentPath = std::filesystem::current_path();;
	//currentPath.append("chess.obj");
	currentPath.append("sponza_tex.obj");

	//GameObjects gameObjects =
	//{
	//	std::make_pair( "Chess_Board_Mesh", "Chess_Board_BaseMap.bmp" ),
	//	std::make_pair( "Pawn_3_Dark_Mesh", "Chess_Pieces_BaseMap.bmp" ),
	//	std::make_pair( "Bishop_Dark_Mesh", "Chess_Pieces_BaseMap.bmp" ),
	//	std::make_pair( "Tower_2_Dark_Mesh", "Chess_Pieces_BaseMap.bmp" ),
	//	std::make_pair( "Queen_Dark_Mesh", "Chess_Pieces_BaseMap.bmp" ),
	//	std::make_pair( "King_Dark_Mesh", "Chess_Pieces_BaseMap.bmp" ),
	//	std::make_pair( "Knight_2_Dark_Mesh", "Chess_Pieces_BaseMap.bmp" ),
	//	std::make_pair( "Knight_Dark_Mesh", "Chess_Pieces_BaseMap.bmp" ),
	//	std::make_pair( "Tower_Dark_Mesh", "Chess_Pieces_BaseMap.bmp" ),
	//	std::make_pair( "Bishop_2_Dark_Mesh", "Chess_Pieces_BaseMap.bmp" ),
	//	std::make_pair( "Pawn_2_Dark_Mesh", "Chess_Pieces_BaseMap.bmp" ),
	//	std::make_pair( "Pawn_4_Dark_Mesh", "Chess_Pieces_BaseMap.bmp" ),
	//	std::make_pair( "Pawn_5_Dark_Mesh", "Chess_Pieces_BaseMap.bmp" ),
	//	std::make_pair( "Pawn_6_Dark_Mesh", "Chess_Pieces_BaseMap.bmp" ),
	//	std::make_pair( "Pawn_7_Dark_Mesh", "Chess_Pieces_BaseMap.bmp" ),
	//	std::make_pair( "Pawn_8_Dark_Mesh", "Chess_Pieces_BaseMap.bmp" ),
	//	std::make_pair( "Pawn_Dark_Mesh", "Chess_Pieces_BaseMap.bmp" ),
	//	std::make_pair( "Pawn_3_Light_Mesh", "Chess_Pieces_BaseMap.bmp" ),
	//	std::make_pair( "Bishop_Light_Mesh", "Chess_Pieces_BaseMap.bmp" ),
	//	std::make_pair( "Tower_2_Light_Mesh", "Chess_Pieces_BaseMap.bmp" ),
	//	std::make_pair( "Queen_Light_Mesh", "Chess_Pieces_BaseMap.bmp" ),
	//	std::make_pair( "King_Light_Mesh", "Chess_Pieces_BaseMap.bmp" ),
	//	std::make_pair( "Knight_2_Light_Mesh", "Chess_Pieces_BaseMap.bmp" ),
	//	std::make_pair( "Knight_Light_Mesh", "Chess_Pieces_BaseMap.bmp" ),
	//	std::make_pair( "Tower_Light_Mesh", "Chess_Pieces_BaseMap.bmp" ),
	//	std::make_pair( "Bishop_2_Light_Mesh", "Chess_Pieces_BaseMap.bmp" ),
	//	std::make_pair( "Pawn_2_Light_Mesh", "Chess_Pieces_BaseMap.bmp" ),
	//	std::make_pair( "Pawn_4_Light_Mesh", "Chess_Pieces_BaseMap.bmp" ),
	//	std::make_pair( "Pawn_5_Light_Mesh", "Chess_Pieces_BaseMap.bmp" ),
	//	std::make_pair( "Pawn_6_Light_Mesh", "Chess_Pieces_BaseMap.bmp" ),
	//	std::make_pair( "Pawn_7_Light_Mesh", "Chess_Pieces_BaseMap.bmp" ),
	//	std::make_pair( "Pawn_8_Light_Mesh", "Chess_Pieces_BaseMap.bmp" ),
	//	std::make_pair( "Pawn_Light_Mesh", "Chess_Pieces_BaseMap.bmp" )
	//};

	GameObjects gameObjects =
	{
		std::make_pair("arcs_01_Mesh", "sp_luk.bmp"),
		std::make_pair("arcs_02_Mesh", "sp_luk.bmp"),
		std::make_pair("arcs_03_Mesh", "sp_luk.bmp"),
		std::make_pair("arcs_04_Mesh", "sp_luk.bmp"),
		std::make_pair("arcs_floo0_Mesh", "sp_luk.bmp"),
		std::make_pair("arcs_floor_Mesh", "sp_luk.bmp"),
		std::make_pair("arcs_long_Mesh", "sp_luk.bmp"),
		std::make_pair("arcs_small_Mesh", "sp_luk.bmp"),
		std::make_pair("ceiling_Mesh", "KAMEN-stup.bmp"),
		std::make_pair("doors_Mesh", "vrata_ko.bmp"),
		std::make_pair("holes_Mesh", "x01_st.bmp"),
		std::make_pair("object19_Mesh", "01_St_kp.bmp"),
		std::make_pair("object21_Mesh", "KAMEN-stup.bmp"),
		std::make_pair("object23_Mesh", "00_skap.bmp"),
		std::make_pair("object27_Mesh", "01_St_kp.bmp"),
		std::make_pair("object28_Mesh", "01_S_ba.bmp"),
		std::make_pair("object3_Mesh", "01_S_ba.bmp"),
		std::make_pair("object31_Mesh", "01_S_ba.bmp"),
		std::make_pair("object32_Mesh", "00_skap.bmp"),
		std::make_pair("object4_Mesh", "01_St_kp.bmp"),
		std::make_pair("object5_Mesh", "00_skap.bmp"),
		std::make_pair("object6_Mesh", "00_skap.bmp"),
		std::make_pair("outside01_Mesh", "KAMEN.bmp"),
		std::make_pair("parapet_Mesh", "sp_luk.bmp"),
		std::make_pair("pillar_cor_Mesh", "01_STUB.bmp"),
		std::make_pair("pillar_flo_Mesh", "01_STUB.bmp"),
		std::make_pair("pillar_qua_Mesh", "sp_01_stub.bmp"),
		std::make_pair("pillar_rou_Mesh", "x01_st.bmp"),
		std::make_pair("puillar_fl_Mesh", "01_STUB.bmp"),
		std::make_pair("relief_Mesh", "reljef.bmp"),
		std::make_pair("round_hole_Mesh", "sp_luk.bmp"),
		std::make_pair("walls_Mesh", "KAMEN.bmp"),
		std::make_pair("windows_Mesh", "prozor1.bmp")
	};

	LoadAssets(gameObjects, currentPath);

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
