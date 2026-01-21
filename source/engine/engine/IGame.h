#pragma once
#include <string>

class Scene;
class IInput;
class Picker;
class RenderContext;
class Camera;

struct EngineServices
{
      // --- Core runtime services (always present) ---

      // Scene graph / entity registry
      Scene* scene = nullptr;

      // Raw input access (mouse, keyboard)
      IInput* input = nullptr;

      // Main active camera (optional but extremely useful for games)
      Camera* camera = nullptr;

      // --- Optional / incremental services ---

      // Picking / raycast (entity + world position)
      Picker* picker = nullptr;

      // Rendering interface (only if game needs to spawn / modify renderables)
      RenderContext* render = nullptr;
};

class IGame
{
public:
      virtual ~IGame() = default;

      // Engine uses this to decide what scene/config to load at startup.
      // Keep this aligned with your current LoadSceneAssets(sceneName) flow.
      virtual std::string GetStartupSceneName() const = 0;

      // Called once when the engine is ready for the game to start.
      // Recommended timing: after scene/assets are loaded and before GameLoop begins.
      virtual void OnInit(EngineServices& services) = 0;

      // Called every frame.
      // dtSeconds is the time since the previous frame.
      virtual void OnUpdate(float dtSeconds) = 0;

      // Called once right before engine shutdown (or when exiting to another app/scene).
      virtual void OnShutdown() = 0;
};