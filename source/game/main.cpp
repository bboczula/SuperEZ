#include "../engine/engine/Engine.h"
#include "../engine/engine/IGame.h"
#include "../engine/engine/IInput.h"
#include "../engine/engine/IScene.h"
#include "../engine/engine/SceneService.h"
#include "../engine/engine/Components.h"
#include <iostream>

class ChessApp final : public IGame
{
public:
	virtual void OnInit(EngineServices& services) override
	{
		// Initialization logic here
		this->services = services;
	}

	virtual void OnUpdate(const FrameTime& frameTime) override
	{
		EntityId piece1Id = services.scene->FindEntityByName("Piece1_Mesh");
		EntityId piece2Id = services.scene->FindEntityByName("Piece2_Mesh");
		std::cout << "1: " << piece1Id << ", 2: " << piece2Id << "\n";
	}



	virtual void OnShutdown() override
	{
		// Shutdown logic here
	}

	std::string GetStartupSceneName() const override
	{
		return "slider_1"; // corresponds to assets/chess/chess.xml in your scheme
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