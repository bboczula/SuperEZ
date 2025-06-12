#pragma once
#include <Windows.h>
#include <vector>
#include <filesystem>
#include <string>
#include "../externals/TinyXML2/tinyxml2.h"

using GameObjects = std::vector<std::pair<std::string, std::string>>;

class Engine
{
public:
	Engine();
	~Engine();
	void Initialize();
	void CreateRenderResources();
	void LoadAssets(GameObjects gameObjects, std::filesystem::path currentPath);
	void Tick();
	void Run();
};