#pragma once

#include <vector>
#include <filesystem>
#include <string>

#include "states/IEngineState.h"

using GameObjects = std::vector<std::pair<std::string, std::string>>;

class Engine
{
public:
	Engine();
	~Engine();
	void Initialize();
	void CreateRenderResources();
	void LoadAssets(GameObjects gameObjects, std::filesystem::path currentPath);
	void ProcessScene(GameObjects& gameObjects, std::filesystem::path& currentPath, const char* sceneName);
	void Tick();
	void Run();
	void ProcessSingleFrame();
	void LoadSceneAssets(std::string sceneName);
	void UnloadSceneAssets();
	void ChangeState(IEngineState* newState);
	void ProcessGlobalCommands();
private:
	IEngineState* currentState = nullptr;
};