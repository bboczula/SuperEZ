#include "../engine/engine/Engine.h"
#include "../engine/engine/IGame.h"
#include <iostream>

class ChessApp final : public IGame
{
public:
	virtual void OnInit(EngineServices& services) override
	{
		// Initialization logic here
	}

	virtual void OnUpdate(float dtSeconds) override
	{
		// Update logic here
		std::cout << "ChessApp OnUpdate: " << dtSeconds << " seconds\n";
	}

	virtual void OnShutdown() override
	{
		// Shutdown logic here
	}

	std::string GetStartupSceneName() const override
	{
		return "chess"; // corresponds to assets/chess/chess.xml in your scheme
	}
};

int main(int argc, char *argv[])
{
	Engine engine;
	engine.CreateRenderResources();

	ChessApp app;
	engine.Run(app);
	return 0;
}