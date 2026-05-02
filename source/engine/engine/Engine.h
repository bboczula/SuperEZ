#pragma once

#include <vector>
#include <filesystem>
#include <string>

#include "states/IEngineState.h"
#include "Coordinator.h"
#include "camera/Camera.h"
#include "../../externals/TinyXML2/tinyxml2.h"
#include "../../externals/SimpleMath/SimpleMath.h"

struct GameObjectData
{
	std::string name;
	std::string meshName;
	std::string textureName;
	DirectX::SimpleMath::Vector3 position;
	DirectX::SimpleMath::Vector3 rotation;
	DirectX::SimpleMath::Vector3 scale;
};

using GameObjects = std::vector<GameObjectData>;


struct CameraData
{
	std::string name;
	DirectX::SimpleMath::Vector3 position;
	DirectX::SimpleMath::Vector3 rotation;
	Camera::CameraType type = Camera::CameraType::PERSPECTIVE;
	float width = 1.0f;
	float height = 1.0f;
	bool active = false;
};

using Cameras = std::vector<CameraData>;

struct SunlightData
{
	std::string name;
	bool enabled = true;
	DirectX::SimpleMath::Vector3 direction = { -0.4f, -1.0f, -0.3f };
	DirectX::SimpleMath::Vector3 color = { 1.0f, 0.98f, 0.92f };
	float ambientStrength = 0.2f;
	float diffuseStrength = 1.0f;
	float shadowBias = 0.001f;
	float shadowSlopeBias = 0.002f;
};

using Sunlights = std::vector<SunlightData>;

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
	void LoadAssets(GameObjects gameObjects, Cameras cameras, Sunlights sunlights, std::filesystem::path currentPath);
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
	void ProcessScene(GameObjects& gameObjects, Cameras& cameras, Sunlights& sunlights, SceneData& sceneData);
	void ProcessCameras(tinyxml2::XMLElement* scene, Cameras& cameras);
	void ProcessSunlights(tinyxml2::XMLElement* scene, Sunlights& sunlights);
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
