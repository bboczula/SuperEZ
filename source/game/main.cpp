#include "../engine/engine/Engine.h"
#include "../engine/engine/IGame.h"
#include "../engine/engine/IInput.h"
#include "../engine/engine/IScene.h"
#include "../engine/engine/SceneService.h"
#include "../engine/engine/Components.h"
#include <iostream>

static bool pendingSelectionAction = false;

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
			boardState[i] = pieceId;
		}
		initialPositions[15] = Vec3{ initialPositions[11].x, 0.0f, initialPositions[14].z };
		boardState[15] = 0xffffffff; // empty
	}

	virtual void OnUpdate(const FrameTime& frameTime) override
	{
		if(services.input->MouseClicked(InputMouseButton::Left))
		{
			pendingSelectionAction = true;  // request started
			return;
		}

		if (!pendingSelectionAction)
		{
			return; // no action requested
		}

		// Piece ID start from 0, and the 0xffffffff means no selection
		EntityId pieceId = services.scene->GetSelectedEntity();
		if (pieceId == 0xffffffff)
		{
			pendingSelectionAction = false; // reset request
			return;
		}
		std::cout << "Selected piece: " << pieceId << std::endl;

		if (!IsPieceEntity(pieceId))
		{
			pendingSelectionAction = false; // reset request
			return;
		}
		std::cout << "It's a valid piece entity." << std::endl;

		auto boardPosition = GetPieceIndexOnBoard(pieceId);
		std::cout << "Piece is at position index: " << boardPosition << std::endl;

		if(IsMoveValid(pieceId))
		{
			auto emptyPosition = GetPieceIndexOnBoard(0xffffffff);
			std::cout << "Move is valid. Moving piece to empty position index: " << emptyPosition << std::endl;
			// Swap positions in board state
			boardState[emptyPosition] = pieceId;
			boardState[boardPosition] = 0xffffffff;

			// Update piece position in scene
			Coordinator* coordinator = services.scene->GetCoordinator();
			auto& transform = coordinator->GetComponent<TransformComponent>(pieceId);
			transform.position[0] = initialPositions[emptyPosition].x;
			transform.position[1] = initialPositions[emptyPosition].y;
			transform.position[2] = initialPositions[emptyPosition].z;
		}
		else
		{
			std::cout << "Move is invalid. Resetting piece position." << std::endl;
		}
		pendingSelectionAction = false; // reset request
	}

	virtual void OnShutdown() override
	{
		// Shutdown logic here
	}

	bool IsPieceEntity(EntityId id) const
	{
		for (unsigned int i = 0; i < boardState.size(); i++)
		{
			if (boardState[i] == id)
				return true;
		}
		return false;
	}

	bool IsMoveValid(EntityId pieceId) const
	{
		auto boardPosition = GetPieceIndexOnBoard(pieceId);
		auto top = boardPosition - 4;
		auto bottom = boardPosition + 4;
		auto left = boardPosition - 1;
		auto right = boardPosition + 1;

		if (top < 16 && boardState[top] == 0xffffffff)
			return true;
		if (bottom < 16 && boardState[bottom] == 0xffffffff)
			return true;
		if (left < 16 && boardState[left] == 0xffffffff)
			return true;
		if (right < 16 && boardState[right] == 0xffffffff)
			return true;
		return false;
	}

	unsigned int GetPieceIndexOnBoard(EntityId pieceId) const
	{
		for(unsigned int i = 0; i < boardState.size(); i++)
		{
			if(boardState[i] == pieceId)
				return i;
		}
		return 0xffffffff; // invalid
	}

	std::string GetStartupSceneName() const override
	{
		return "slider_1"; // corresponds to assets/chess/chess.xml in your scheme
	}
private:
	EngineServices services;
	std::array<Vec3, 16> initialPositions;
	std::array<EntityId, 16> boardState;
};

int main(int argc, char *argv[])
{
	Engine engine;
	engine.CreateRenderResources();

	ChessApp app;
	engine.Run(app);
	return 0;
}