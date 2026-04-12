#include "../engine/engine/Engine.h"
#include "../engine/engine/IGame.h"
#include "../engine/engine/IInput.h"
#include "../engine/engine/IScene.h"
#include "../engine/engine/SceneService.h"
#include "../engine/engine/Components.h"
#include <iostream>
#include <random>
#include <chrono>

static bool pendingSelectionAction = false;

class SceneViewer final : public IGame
{
public:
	virtual void OnInit(EngineServices& services) override
	{
	}


	virtual void OnUpdate(const FrameTime& frameTime) override
	{
	}

	virtual void OnShutdown() override
	{
		// Shutdown logic here
	}

	std::string GetStartupSceneName() const override
	{
		return "slider_1"; // corresponds to assets/chess/chess.xml in your scheme
	}
};

int main(int argc, char *argv[])
{
	Engine engine;
	engine.CreateRenderResources();

	SceneViewer app;
	engine.Run(app);
	return 0;
}
