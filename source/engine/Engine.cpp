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
	renderContext.CreateDefaultSamplers();
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
	//currentPath.append("spheres.obj");
	//currentPath.append("cube.obj");
	//currentPath.append("temple.obj");
	//currentPath.append("chess.obj");
	//currentPath.append("bunny.obj");
	currentPath.append("sponza_tex.obj");

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

	//std::vector < std::pair < std::string, std::string>> gameObjects =
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

	std::vector < std::pair < std::string, std::string>> gameObjects =
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
		
	//std::vector<std::string> meshNames = {
	//	"bunny_Mesh"
	//};

	//std::vector<std::string> meshNames = {
	//	"Sphere_2_Mesh",
	//	"Sphere_2.001_Mesh",
	//	"Sphere_2.002_Mesh"
	//};
	//
	//std::vector<std::string> textureNames = {
	//	"moon_texture.bmp",
	//	"earth_texture.bmp",
	//	"mars_texture.bmp",
	//};

	//std::vector<std::string> meshNames = {
	//	"Cube_Mesh"
	//};

	// Load Geometry
	for (const auto& gameObject : gameObjects)
	{
		const auto& meshName = gameObject.first;
		const auto& textureName = gameObject.second;

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

		errorCode = assetManager.MeshGet(meshName.c_str(), AssetSuite::MeshOutputFormat::TEXCOORD, meshOutput, meshDescriptor);
		if (errorCode != AssetSuite::ErrorCode::OK)
		{
			OutputDebugString(L"Failed to load mesh: ");
			OutputDebugStringA(meshName.c_str());
			OutputDebugString(L"\n");
			continue;
		}
		CHAR tempNameTexture[64];
		snprintf(tempNameTexture, sizeof(tempNameTexture), "TEXCOORD_%s", meshName.c_str());
		auto vbIndexTexture = renderContext.CreateVertexBuffer(numOfTriangles * 3, 2, meshOutput.data(), tempNameTexture);

		renderContext.CreateMesh(vbIndexPositionAndColor, vbIndexColor, vbIndexTexture, meshName.c_str());

		std::filesystem::path currentTexturePath = std::filesystem::current_path();
		currentTexturePath.append(textureName);
		assetManager.ImageLoadAndDecode(currentTexturePath.string().c_str());

		std::vector<BYTE> imageOutput;
		AssetSuite::ImageDescriptor imageDescriptor = {};
		assetManager.ImageGet(AssetSuite::OutputFormat::RGB8, imageOutput, imageDescriptor);

		CHAR tempNameTextureImage[64];
		snprintf(tempNameTextureImage, sizeof(tempNameTextureImage), "TEX_%s", meshName.c_str());
		renderContext.CreateTexture(imageDescriptor.width, imageDescriptor.height, imageOutput.data(), tempNameTextureImage);
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
