#pragma once

#include <vector>
#include <filesystem>
#include <string>
#include <map>
#include "../asset/Handle.h"

namespace AssetSuite { class Manager; }

#include "states/IEngineState.h"
#include "Coordinator.h"
#include "camera/Camera.h"
#include "../renderer/ColorPipeline.h"
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
	std::string GetCurrentSceneName() const { return currentSceneName; }
	std::string GetCurrentMeshLibraryFile() const { return currentMeshLibraryFile; }
	const std::vector<std::string>& GetMeshLibraryGroups() const { return meshLibraryGroups; }
	const std::string& GetMeshLibraryError() const { return meshLibraryError; }
	const std::vector<std::string>& GetLibraryTextures() const { return libraryTextures; }
	std::string GetLibraryTexture(const std::string& meshName) const;
	bool SetLibraryTexture(const std::string& meshName, const std::string& textureName);
	void QueueLibraryAsset(const std::string& meshName);
	bool CanDuplicateObject(Entity entity);
	void QueueDuplicateObject(Entity entity);
	const std::string& GetDuplicateError() const { return duplicateError; }
	const std::string& GetLibraryInsertionError() const { return libraryInsertionError; }
private:
	// Handle scene XML processing
	bool PrepareObjectResources(AssetSuite::Manager& manager, const GameObjectData& object, HMesh& mesh, HTexture& texture);
	Entity CreateSceneObject(const GameObjectData& object, HMesh mesh, HTexture texture);
	void AddLibraryAsset(const std::string& meshName, const std::string& textureName);
	void DuplicateObject(Entity entity);
	void ProcessScene(GameObjects& gameObjects, Cameras& cameras, Sunlights& sunlights, SceneData& sceneData);
	void ProcessCameras(tinyxml2::XMLElement* scene, Cameras& cameras);
	void ProcessSunlights(tinyxml2::XMLElement* scene, Sunlights& sunlights);
	void ProcessGameObjects(tinyxml2::XMLElement* scene, GameObjects& gameObjects);
private:
	IGame* game = nullptr;
	IEngineState* currentState = nullptr;
	std::string startupSceneName;
	std::string currentSceneName;
	std::string currentMeshLibraryFile;
	std::vector<std::string> meshLibraryGroups;
	std::string meshLibraryError;
	std::vector<std::string> libraryTextures;
	std::map<std::string, std::string> libraryTextureAssignments;
	std::map<std::string, HMesh> loadedMeshes;
	std::map<std::string, HTexture> loadedTextures;
	std::filesystem::path modelLibraryPath;
	std::string libraryInsertionError;
	std::string duplicateError;
	uint64_t sceneGeneration = 0;
	RawInputService* rawInputService = nullptr;
	SceneService* sceneService = nullptr;
	TimeSystem* timeSystem = nullptr;
	Coordinator mCoordinator;
	RenderService* renderService = nullptr;
};
