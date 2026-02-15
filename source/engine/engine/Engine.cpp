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
#include "IGame.h"
#include "RawInputService.h"
#include "SceneService.h"
#include "RenderService.h"
#include "IInput.h"
#include "IScene.h"
#include "TimeSystem.h"
#include "Components.h"
#include "ECSTypes.h"

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

// Utility functions for logging
inline std::ostream& operator<<(std::ostream& os, const DirectX::SimpleMath::Vector3& v)
{
	return os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
}

inline std::ostream& operator<<(std::ostream& os, const CameraData& c)
{
	return os << "CameraData { " << "position: " << c.position << ", " << "rotation: " << c.rotation << " }";
}

inline std::ostream& operator<<(std::ostream& os, const GameObjectData& c)
{
	return os << "GameObjectData { " << "name: " << c.name << ", " << "mesh: "
		<< c.meshName << ", " << "texture: " << c.textureName << ", " << "position: "
		<< c.position << ", " << "rotation: " << c.rotation << ", " << "scale: " << c.scale << " }";
}

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

	// --- 1. ECS INITIALIZATION START ---
	mCoordinator.Init();

	// Register the components we defined in Step 1
	// (You MUST register them before using them)
	mCoordinator.RegisterComponent<TransformComponent>();
	mCoordinator.RegisterComponent<GeometryComponent>();
	mCoordinator.RegisterComponent<MaterialComponent>();
	mCoordinator.RegisterComponent<InfoComponent>();

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
	sceneService = new SceneService(renderContext, &mCoordinator);
	renderService = new RenderService();

	timeSystem = new TimeSystem();
	timeSystem->SetMaxDeltaSeconds(0.1);
	timeSystem->SetTimeScale(1.0);
}

void Engine::CreateRenderResources()
{
	// Here we can make engine speciffic allocations
}

void Engine::LoadAssets(GameObjects gameObjects, CameraData cameraData, std::filesystem::path currentPath)
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

	const auto aspectRatio = static_cast<float>(windowContext.GetWidth()) / static_cast<float>(windowContext.GetHeight());
	renderContext.CreateCamera(aspectRatio, cameraData.position, cameraData.rotation);

	for (const auto& gameObject : gameObjects)
	{
		const auto& meshName = gameObject.meshName;
		const auto& textureName = gameObject.textureName;

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
		const uint32_t id = meshIndex;

		RenderItem item{};
		item.id = id;
		item.position = gameObject.position;
		item.rotation = gameObject.rotation;
		item.scale = gameObject.scale;
		item.mesh = HMesh(id);
		item.texture = HTexture(id);
		strncpy_s(item.name, gameObject.name.c_str(), _TRUNCATE);

		renderContext.CreateRenderItem(item);

		// Create the entity in the ECS
		renderService->CreateEntity(mCoordinator, item);
	}

	EngineServices services
	{
		.scene = sceneService,
		.input = rawInputService,
		.camera = nullptr,
		.picker = nullptr,
		.render = renderService
	};
	game->OnInit(services);
}

void Engine::ProcessScene(GameObjects& gameObjects, CameraData& cameraData, SceneData& sceneData)
{
	sceneData.currentPath.append(sceneData.sceneName);
	std::filesystem::path scenePath = sceneData.currentPath / (std::string(sceneData.sceneName) + ".xml");

	tinyxml2::XMLDocument document;
	tinyxml2::XMLError xmlError = document.LoadFile(scenePath.string().c_str());
	assert(xmlError == tinyxml2::XML_SUCCESS, "Failed to load the scene XML file!");

	tinyxml2::XMLElement* scene = document.FirstChildElement("Scene");
	assert(scene != nullptr, "Scene element not found in the scene XML file!");

	tinyxml2::XMLElement* geometryLibrary = scene->FirstChildElement("MeshLibrary");
	assert(geometryLibrary != nullptr, "MeshLibrary element not found in the scene XML file!");

	const char* geometryLibraryFile = geometryLibrary->Attribute("file");
	assert(geometryLibraryFile != nullptr, "GeometryLibrary 'file' attribute not found in the scene XML file!");
	sceneData.currentPath.append(geometryLibraryFile);

	ProcessGameObjects(scene, gameObjects);
	ProcessCamera(scene, cameraData);
}

void Engine::ProcessCamera(tinyxml2::XMLElement* scene, CameraData& cameraData)
{
	tinyxml2::XMLElement* cameraElem = scene->FirstChildElement("Camera");
	assert(cameraElem != nullptr, "Camera element not found in scene XML file!");
	const char* name = cameraElem->Attribute("name");

	tinyxml2::XMLElement* cameraPosition = cameraElem->FirstChildElement("Position");
	assert(cameraPosition != nullptr, "Camera position element not found in scene XML file!");
	cameraData.position.x = cameraPosition->FloatAttribute("x", 0.0f);
	cameraData.position.y = cameraPosition->FloatAttribute("y", 0.0f);
	cameraData.position.z = cameraPosition->FloatAttribute("z", 0.0f);

	tinyxml2::XMLElement* cameraRotation = cameraElem->FirstChildElement("Rotation");
	assert(cameraRotation != nullptr, "Camera rotation element not found in scene XML file!");
	cameraData.rotation.x = cameraRotation->FloatAttribute("pitch", 0.0f);
	cameraData.rotation.y = cameraRotation->FloatAttribute("yaw", 0.0f);
	cameraData.rotation.z = cameraRotation->FloatAttribute("roll", 0.0f);
	
	std::cout << "[Camera] " << cameraData << "\n";
}

void Engine::ProcessGameObjects(tinyxml2::XMLElement* scene, GameObjects& gameObjects)
{
	for (tinyxml2::XMLElement* go = scene->FirstChildElement("GameObject"); go != nullptr; go = go->NextSiblingElement("GameObject"))
	{
		const char* name = go->Attribute("name");
		const char* mesh = go->Attribute("mesh");
		const char* texture = go->Attribute("texture");

		GameObjectData gameObject;
		gameObject.name = name ? name : "default_name";
		gameObject.meshName = mesh ? mesh : "default_mesh";
		gameObject.textureName = texture ? texture : "default_texture";

		tinyxml2::XMLElement* position = go->FirstChildElement("Position");
		assert(position != nullptr, "GameObject position element not found in scene XML file!");
		gameObject.position.x = position->FloatAttribute("x", 0.0f);
		gameObject.position.y = position->FloatAttribute("y", 0.0f);
		gameObject.position.z = position->FloatAttribute("z", 0.0f);

		tinyxml2::XMLElement* rotation = go->FirstChildElement("Rotation");
		assert(rotation != nullptr, "GameObject rotation element not found in scene XML file!");
		gameObject.rotation.x = rotation->FloatAttribute("pitch", 0.0f);
		gameObject.rotation.y = rotation->FloatAttribute("yaw", 0.0f);
		gameObject.rotation.z = rotation->FloatAttribute("roll", 0.0f);

		tinyxml2::XMLElement* scale = go->FirstChildElement("Scale");
		assert(scale != nullptr, "GameObject scale element not found in scene XML file!");
		gameObject.scale.x = scale->FloatAttribute("x", 1.0f);
		gameObject.scale.y = scale->FloatAttribute("y", 1.0f);
		gameObject.scale.z = scale->FloatAttribute("z", 1.0f);

		gameObjects.emplace_back(gameObject);
		std::cout << "[GameObject] " << gameObject << "\n";
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
	sceneService->Update(frameTime.dt);
	renderService->Update(mCoordinator);

	Tick();

	winMessageSubject.RunPostFrame();
}

void Engine::LoadSceneAssets(std::string sceneName)
{
	GameObjects gameObjects;
	CameraData cameraData;
	SceneData sceneData =
	{
		std::filesystem::current_path() / "assets",
		sceneName.c_str()
	};
	ProcessScene(gameObjects, cameraData, sceneData);
	LoadAssets(gameObjects, cameraData, sceneData.currentPath);
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

void Engine::PostLoadAssets()
{
	renderGraph.PostAssetLoad();
}
