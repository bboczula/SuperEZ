#include "../engine/engine/Engine.h"
#include "../engine/engine/IGame.h"
#include "../engine/engine/IInput.h"
#include "../engine/engine/IScene.h"
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
		auto moon = services.scene->FindEntityByName("Earth_Mesh");
		auto pos = services.scene->GetPosition(moon);
		if(services.input->IsKeyDown('U'))
		{
			pos.y += 0.001f;
		}
		if(services.input->IsKeyDown('J'))
		{
			pos.y -= 0.001f;
		}
		services.scene->SetPosition(moon, pos);
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