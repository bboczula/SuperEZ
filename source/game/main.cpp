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
			pieceEntities[i] = pieceId;
			initialPositions[i] = services.scene->GetPosition(pieceId);
			boardState[i] = i + 1; // 0 means empty
			piecePosition[i + 1] = pieceId;
		}
		initialPositions[15] = Vec3{ initialPositions[11].x, 0.0f, initialPositions[14].z };
		boardState[15] = 0; // empty
		piecePosition[0] = pieceEntities[15]; // empty position
	}

	virtual void OnUpdate(const FrameTime& frameTime) override
	{
		// Piece ID start from 0, and the 0xffffffff means no selection
		EntityId pieceId = services.scene->GetSelectedEntity();
		if (pieceId == 0xffffffff)
			return;
		std::cout << "Selected piece: " << pieceId << std::endl;

		if (!IsPieceEntity(pieceId))
			return;
		std::cout << "It's a valid piece entity." << std::endl;

		auto i = piecePosition[pieceId];
		std::cout << "Piece is at position index: " << i << std::endl;

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

	bool IsPieceEntity(EntityId id) const
	{
		for(auto pieceId : pieceEntities)
		{
			if (pieceId == id)
				return true;
		}
		return false;
	}

	bool IsMoveValid(EntityId pieceId) const
	{
		// I need to know which piece is being moved and where
		auto i = piecePosition[pieceId];
		
		// There are some corner cases
		if(i % 4 == 0) // left edge
		{
			if(boardState[i + 1] != 0) // left is occupied
				return false;
		}
	}

	std::string GetStartupSceneName() const override
	{
		return "slider_1"; // corresponds to assets/chess/chess.xml in your scheme
	}
private:
	EngineServices services;
	std::array<Vec3, 16> initialPositions;
	std::array<EntityId, 16> pieceEntities;
	std::array<EntityId, 16> piecePosition;
	std::array<unsigned int, 16> boardState;
};

int main(int argc, char *argv[])
{
	Engine engine;
	engine.CreateRenderResources();

	ChessApp app;
	engine.Run(app);
	return 0;
}