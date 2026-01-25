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

		for (int i = 0; i < 15; i++)
		{
			EntityId pieceId = services.scene->FindEntityByName("Piece" + std::to_string(i + 1));
			initialPositions[i] = services.scene->GetPosition(pieceId);
		}
		initialPositions[15] = Vec3{ initialPositions[11].x, 0.0f, initialPositions[14].z };
	}

	virtual void OnUpdate(const FrameTime& frameTime) override
	{
		EntityId pieceId = services.scene->FindEntityByName("Piece15");
		Coordinator* coordinator = services.scene->GetCoordinator();
		auto& transform = coordinator->GetComponent<TransformComponent>(pieceId);
		transform.position[0] = initialPositions[15].x;
		transform.position[1] = initialPositions[15].y;
		transform.position[2] = initialPositions[15].z;
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
	std::array<Vec3, 16> initialPositions;
};

int main(int argc, char *argv[])
{
	Engine engine;
	engine.CreateRenderResources();

	ChessApp app;
	engine.Run(app);
	return 0;
}