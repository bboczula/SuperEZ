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

class ChessApp final : public IGame
{
public:
	virtual void OnInit(EngineServices& services) override
	{
		this->services = services;

		for (int i = 0; i < 15; i++)
		{
			EntityId pieceId = services.scene->FindEntityByName("Piece" + std::to_string(i + 1));
			initialPositions[i] = services.scene->GetPosition(pieceId);
			boardState[i] = pieceId;
		}
		initialPositions[15] = Vec3{ initialPositions[11].x, 0.0f, initialPositions[14].z };
		boardState[15] = Empty;

		ShuffleBoard(200);

		goodJobId = services.scene->FindEntityByName("GoodJob");
		goodJobInitialPosition = services.scene->GetPosition(goodJobId);
	}


	virtual void OnUpdate(const FrameTime& frameTime) override
	{
		if(IsBoardSolved())
		{
			std::cout << "Board solved!" << std::endl;
			services.scene->SetPosition(goodJobId, Vec3(goodJobInitialPosition.x, goodJobInitialPosition.y + 0.5f, goodJobInitialPosition.z));
			return;
		}
		
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
			services.scene->SetPosition(pieceId, initialPositions[emptyPosition]);
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
		const unsigned p = GetPieceIndexOnBoard(pieceId);
		const unsigned e = GetPieceIndexOnBoard(Empty);
		if (p == 0xffffffff || e == 0xffffffff) return false;

		const int pr = int(p / 4), pc = int(p % 4);
		const int er = int(e / 4), ec = int(e % 4);

		const int manhattan = std::abs(pr - er) + std::abs(pc - ec);
		return manhattan == 1;
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
	EntityId goodJobId = 0xffffffff;
	std::array<Vec3, 16> initialPositions;
	Vec3 goodJobInitialPosition;
	std::array<EntityId, 16> boardState;
	std::mt19937 rng{ static_cast<uint32_t>(
	  std::chrono::high_resolution_clock::now().time_since_epoch().count()) };

	static constexpr EntityId Empty = 0xffffffff;

	unsigned CountNeighbors(unsigned emptyIdx) const
	{
		unsigned count = 0;
		const unsigned r = emptyIdx / 4;
		const unsigned c = emptyIdx % 4;
		if (r > 0) ++count;
		if (r < 3) ++count;
		if (c > 0) ++count;
		if (c < 3) ++count;
		return count;
	}

	void ApplyBoardStateToScene()
	{
		for (unsigned i = 0; i < 16; ++i)
		{
			const EntityId id = boardState[i];
			if (id == Empty)
			{
				continue;
			}
			services.scene->SetPosition(id, initialPositions[i]);
		}
	}

	void ShuffleBoard(unsigned moves)
	{
		unsigned emptyIdx = GetPieceIndexOnBoard(Empty);
		unsigned prevEmptyIdx = 0xffffffff;

		for (unsigned k = 0; k < moves; ++k)
		{
			std::array<unsigned, 4> candidates{};
			unsigned count = 0;

			const unsigned r = emptyIdx / 4;
			const unsigned c = emptyIdx % 4;

			auto push = [&](unsigned idx)
				{
					if (idx != prevEmptyIdx)
						candidates[count++] = idx;
				};

			if (r > 0) push(emptyIdx - 4);
			if (r < 3) push(emptyIdx + 4);
			if (c > 0) push(emptyIdx - 1);
			if (c < 3) push(emptyIdx + 1);

			if (count == 0)
			{
				if (r > 0) candidates[count++] = emptyIdx - 4;
				if (r < 3) candidates[count++] = emptyIdx + 4;
				if (c > 0) candidates[count++] = emptyIdx - 1;
				if (c < 3) candidates[count++] = emptyIdx + 1;
			}

			std::uniform_int_distribution<unsigned> dist(0, count - 1);
			const unsigned pick = candidates[dist(rng)];

			std::swap(boardState[pick], boardState[emptyIdx]);

			prevEmptyIdx = emptyIdx;
			emptyIdx = pick;
		}

		ApplyBoardStateToScene();
	}

	bool IsBoardSolved() const
	{
		for (unsigned i = 0; i < 15; ++i)
		{
			if (boardState[i] != services.scene->FindEntityByName("Piece" + std::to_string(i + 1)))
				return false;
		}

		return boardState[15] == Empty;
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