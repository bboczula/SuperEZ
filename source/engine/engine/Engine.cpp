#include "Engine.h"

#include "../core/DeviceContext.h"
#include "WindowContext.h"
#include "../renderer/RenderContext.h"
#include "../renderer/RenderGraph.h"
#include "../renderer/RenderItem.h"
#include "Settings.h"
#include "Observer.h"
#include "input/RawInput.h"
#include "input/ImGuiHandler.h"
#include "input/CursorInput.h"
#include "../../externals/AssetSuite/inc/AssetSuite.h"
#include "../../externals/TinyXML2/tinyxml2.h"
#include "IGame.h"
#include "RawInputService.h"
#include "SceneService.h"
#include "IInput.h"
#include "IScene.h"
#include "TimeSystem.h"

#include "states/EngineCommandQueue.h"
#include "states/StartupState.h"
#include "states/LoadAssetsState.h"
#include "states/GameLoopState.h"
#include "states/ExitState.h"
#include "states/UnloadAssetsState.h"

#define FRAME_COUNT 2

// Global Entities
Settings settings;
WindowContext windowContext;
DeviceContext deviceContext;
RenderContext renderContext;
RenderGraph renderGraph;
Subject<WinMessageEvent> winMessageSubject;
RawInput rawInput;
CursorInput cursorInput;
ImGuiHandler imGuiHandler;

Engine::Engine()
{
	OutputDebugString(L"Engine Constructor\n");
	GlobalCommandQueue::Push(EngineCommand{ EngineCommandType::Startup });
}

Engine::~Engine()
{
	OutputDebugString(L"Engine Destructor\n");
}

void Engine::Initialize()
{
	OutputDebugString(L"Engine::Initialize()\n");
	rawInput.Initialize();
	imGuiHandler.Initialize();
	winMessageSubject.Subscribe(&imGuiHandler);
	winMessageSubject.Subscribe(&rawInput);
	winMessageSubject.Subscribe(&cursorInput);
	renderContext.CreateDescriptorHeap(&deviceContext);
	renderContext.CreateDefaultSamplers();
	renderContext.CreateRenderTargetFromBackBuffer(&deviceContext);
	renderGraph.Initialize();

	// Prepare GameServices
	rawInputService = new RawInputService(rawInput);
	sceneService = new SceneService(renderContext);
	EngineServices services
	{
		.scene = sceneService,
		.input = rawInputService,
		.camera = nullptr,
		.picker = nullptr,
		.render = &renderContext
	};

	game->OnInit(services);

	timeSystem = new TimeSystem();
	timeSystem->SetMaxDeltaSeconds(0.1);
	timeSystem->SetTimeScale(1.0);
}

void Engine::CreateRenderResources()
{
	// Here we can make engine speciffic allocations
}

void Engine::LoadAssets(GameObjects gameObjects, std::filesystem::path currentPath)
{
	AssetSuite::Manager assetManager;
	assetManager.MeshLoadAndDecode(currentPath.string().c_str(), AssetSuite::MeshDecoders::WAVEFRONT);

	std::vector<float> meshOutput;
	AssetSuite::MeshDescriptor meshDescriptor{};
	std::vector<uint8_t> imageOutput;
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

		auto CreateTexture = [&](const AssetSuite::ImageDescriptor& desc, std::vector<uint8_t>& data)
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

		//auto texturePath = std::filesystem::current_path() / textureName;
		auto texturePath = currentPath.remove_filename() / textureName;
		assetManager.ImageLoadAndDecode(texturePath.string().c_str());
		assetManager.ImageGet(AssetSuite::OutputFormat::RGB8, imageOutput, imageDescriptor);

		CreateTexture(imageDescriptor, imageOutput);

		// After renderContext.CreateMesh(...)
		const uint32_t meshIndex = renderContext.GetNumOfMeshes() - 1;
		const uint32_t id = meshIndex + 1;

		RenderItem item{};
		item.id = id;
		item.position = { 0.0f, 0.0f, 0.0f };
		item.mesh = HMesh(id - 1);
		item.texture = HTexture(id - 1);
		strncpy_s(item.name, meshName.c_str(), _TRUNCATE);

		renderContext.CreateRenderItem(item);


	}
}

void Engine::ProcessScene(GameObjects& gameObjects, std::filesystem::path& currentPath, const char* sceneName)
{
	currentPath.append(sceneName);
	std::filesystem::path scenePath = currentPath / (std::string(sceneName) + ".xml");

	tinyxml2::XMLDocument doc;
	tinyxml2::XMLError xmlError = doc.LoadFile(scenePath.string().c_str());
	assert(xmlError == tinyxml2::XML_SUCCESS, "Failed to load sponza.xml");

	tinyxml2::XMLElement* scene = doc.FirstChildElement("Scene");
	assert(scene != nullptr, "Scene element not found in sponza.xml");

	tinyxml2::XMLElement* meshLib = scene->FirstChildElement("MeshLibrary");
	if (meshLib)
	{
		const char* meshFile = meshLib->Attribute("file");
		std::cout << "Mesh file: " << (meshFile ? meshFile : "none") << "\n";
		currentPath.append(meshFile);
	}

	for (tinyxml2::XMLElement* go = scene->FirstChildElement("GameObject"); go != nullptr; go = go->NextSiblingElement("GameObject"))
	{
		const char* name = go->Attribute("name");
		const char* mesh = go->Attribute("mesh");
		const char* texture = go->Attribute("texture");

		std::cout << "[GameObject] name: " << (name ? name : "none")
			<< ", mesh: " << (mesh ? mesh : "none")
			<< ", texture: " << (texture ? texture : "none") << "\n";

		gameObjects.emplace_back(std::make_pair(mesh ? mesh : "default_mesh", texture ? texture : "default_texture"));
	}
}

void Engine::Tick()
{
	if (rawInput.IsKeyDown(VK_ESCAPE))
	{
		OutputDebugString(L"Engine::Tick() - Escape key pressed\n");
		GlobalCommandQueue::Push(EngineCommand{ EngineCommandType::Exit });
	}
	renderGraph.Execute();
	deviceContext.Flush();
	deviceContext.Present();
	deviceContext.Flush();
}

void Engine::Run(IGame& game)
{
	startupSceneName = game.GetStartupSceneName();
	this->game = &game;

	while (1)
	{
		ProcessGlobalCommands();
		currentState->Update(*this);
	}
}

void Engine::ProcessSingleFrame()
{
	MSG msg{ 0 };
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			OutputDebugString(L"Engine::Run() - WM_QUIT received\n");
			GlobalCommandQueue::Push(EngineCommand{ EngineCommandType::Exit });
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	const FrameTime& frameTime = timeSystem->Tick();
	game->OnUpdate(frameTime);

	Tick();
	winMessageSubject.RunPostFrame();
}

void Engine::LoadSceneAssets(std::string sceneName)
{
	GameObjects gameObjects;
	std::filesystem::path currentPath = std::filesystem::current_path();
	currentPath.append("assets");
	ProcessScene(gameObjects, currentPath, sceneName.c_str());
	LoadAssets(gameObjects, currentPath);
}

void Engine::UnloadSceneAssets()
{
	renderContext.UnloadAssets();
}

void Engine::ChangeState(IEngineState* newState)
{
	if (currentState)
	{
		currentState->Exit(*this);
		delete currentState;
	}

	currentState = newState;

	if (currentState)
	{
		currentState->Enter(*this);
	}
	else
	{
		OutputDebugString(L"Engine::ChangeState() - No new state provided\n");
	}
}

void Engine::ProcessGlobalCommands()
{
	auto commands = GlobalCommandQueue::Consume();
	while (!commands.empty())
	{
		EngineCommand cmd = commands.front();

		switch (cmd.type)
		{
		case EngineCommandType::Startup:
			ChangeState(new StartupState());
			break;
		case EngineCommandType::LoadAssets:
		{
			// Why do I need braces here???
			auto payload = std::get<LoadAssetsPayload>(cmd.payload);
			ChangeState(new LoadAssetsState(payload.sceneName));
			break;
		}
		case EngineCommandType::GameLoop:
			ChangeState(new GameLoopState());
			break;
		case EngineCommandType::UnloadAssets:
			ChangeState(new UnloadAssetsState());
			break;
		case EngineCommandType::Exit:
			ChangeState(new ExitState());
			break;
		default:
			break;
		}

		commands.pop();
	}
}
