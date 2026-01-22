#include "../engine/engine/Engine.h"
#include "../engine/engine/IGame.h"
#include "../engine/engine/IInput.h"
#include <iostream>

class ChessApp final : public IGame
{
public:
	virtual void OnInit(EngineServices& services) override
	{
		// Initialization logic here
		this->services = services;
	}

	virtual void OnUpdate(float dtSeconds) override
	{
		// Update logic here
		if(services.input->MouseClicked(InputMouseButton::Left))
		{
			std::cout << "Left mouse button clicked!" << std::endl;
		}
	}

	virtual void OnShutdown() override
	{
		// Shutdown logic here
	}

	std::string GetStartupSceneName() const override
	{
		return "milkyway"; // corresponds to assets/chess/chess.xml in your scheme
	}
private:
	EngineServices services;
};

int main(int argc, char *argv[])
{
	Engine engine;
	engine.CreateRenderResources();

	ChessApp app;
	engine.Run(app);
	return 0;
}