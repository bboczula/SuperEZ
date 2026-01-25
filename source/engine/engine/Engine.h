#pragma once

#include <vector>
#include <filesystem>
#include <string>

#include "states/IEngineState.h"
#include "Coordinator.h"

using GameObjects = std::vector<std::pair<std::string, std::string>>;

struct CameraData
{
	float position[3];
	float rotation[3];
};

class IGame;
class RawInputService;
class SceneService;
class TimeSystem;
class RenderService;

class Engine
{
public:
	Engine();
	~Engine();
	void Initialize();
	void CreateRenderResources();
	void LoadAssets(GameObjects gameObjects, CameraData cameraData, std::filesystem::path currentPath);
	void ProcessScene(GameObjects& gameObjects, CameraData& cameraData, std::filesystem::path& currentPath, const char* sceneName);
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
	IGame* game = nullptr;
	IEngineState* currentState = nullptr;
	std::string startupSceneName;
	RawInputService* rawInputService = nullptr;
	SceneService* sceneService = nullptr;
	TimeSystem* timeSystem = nullptr;
	Coordinator mCoordinator;
	RenderService* renderService = nullptr;
};