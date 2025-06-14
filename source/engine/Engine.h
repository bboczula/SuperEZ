#pragma once
#include <Windows.h>
#include <vector>
#include <filesystem>
#include <string>

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
};