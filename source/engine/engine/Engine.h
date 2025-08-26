#pragma once
#include <Windows.h>
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

//class RenderState : public IEngineState
//{
//	public:
//	void Enter(Engine& engine) override
//	{
//		OutputDebugString(L"Entering RenderState\n");
//	}
//	void Exit(Engine& engine) override
//	{
//		OutputDebugString(L"Exiting RenderState\n");
//	}
//	void Update(Engine& engine) override
//	{
//		engine.Tick();
//	}
//};
//
//class LoadAssetsState : public IEngineState
//{
//	public:
//	void Enter(Engine& engine) override
//	{
//		OutputDebugString(L"Entering LoadAssetsState\n");
//	}
//	void Exit(Engine& engine) override
//	{
//		OutputDebugString(L"Exiting LoadAssetsState\n");
//	}
//	void Update(Engine& engine) override
//	{
//		GameObjects gameObjects;
//		std::filesystem::path currentPath = "Assets/Scenes/Scene1";
//		engine.LoadAssets(gameObjects, currentPath);
//		engine.ProcessScene(gameObjects, currentPath, "Scene1");
//		engine.ChangeState(new StartupState());
//	}
//};