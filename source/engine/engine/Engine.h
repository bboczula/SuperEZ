#pragma once

#include <vector>
#include <filesystem>
#include <string>

#include "states/IEngineState.h"

#define IS_EDITOR 0

using GameObjects = std::vector<std::pair<std::string, std::string>>;

class IGame;

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
	void Run(IGame& game);
	void ProcessSingleFrame();
	void LoadSceneAssets(std::string sceneName);
	void UnloadSceneAssets();
	void ChangeState(IEngineState* newState);
	void ProcessGlobalCommands();
	std::string GetStartupSceneName() const { return startupSceneName; }
private:
	IGame* game = nullptr;
	IEngineState* currentState = nullptr;
	std::string startupSceneName;
};