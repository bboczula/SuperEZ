#pragma once

#include <vector>
#include <filesystem>
#include <string>

#include "states/IEngineState.h"
#include "Coordinator.h"
#include "../../externals/TinyXML2/tinyxml2.h"
#include "../../externals/SimpleMath/SimpleMath.h"

using GameObjects = std::vector<std::pair<std::string, std::string>>;

struct CameraData
{
	DirectX::SimpleMath::Vector3 position;
	DirectX::SimpleMath::Vector3 rotation;
};

struct SceneData
{
	std::filesystem::path currentPath;
	const char* sceneName;
};

class IGame;
class RawInputService;
class SceneService;
class TimeSystem;
class RenderService;
class tinyxml2::XMLElement;

class Engine
{
public:
	Engine();
	~Engine();
	void Initialize();
	void CreateRenderResources();
	void LoadAssets(GameObjects gameObjects, CameraData cameraData, std::filesystem::path currentPath);
	void Tick();
	void Run(IGame& game);
	void ProcessSingleFrame();
	void LoadSceneAssets(std::string sceneName);
	void UnloadSceneAssets();
	void ChangeState(IEngineState* newState);
	void ProcessGlobalCommands();
	void PostLoadAssets();
	std::string GetStartupSceneName() const { return startupSceneName; }
private:
	// Handle scene XML processing
	void ProcessScene(GameObjects& gameObjects, CameraData& cameraData, SceneData& sceneData);
	void ProcessCamera(tinyxml2::XMLElement* scene, CameraData& cameraData);
	void ProcessGameObjects(tinyxml2::XMLElement* scene, GameObjects& gameObjects);
private:
	IGame* game = nullptr;
	IEngineState* currentState = nullptr;
	std::string startupSceneName;
	RawInputService* rawInputService = nullptr;
	SceneService* sceneService = nullptr;
	TimeSystem* timeSystem = nullptr;
	Coordinator mCoordinator;
	RenderService* renderService = nullptr;
};