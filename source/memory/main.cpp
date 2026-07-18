#include "../engine/engine/Engine.h"
#include "../engine/engine/IGame.h"
#include "../engine/engine/IInput.h"
#include "../engine/engine/IScene.h"
#include "../engine/engine/SceneService.h"
#include "../engine/engine/Components.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <random>

namespace
{
	constexpr std::size_t TileColumns = 5;
	constexpr std::size_t TileRows = 4;
	constexpr float TileSize = 1.0f;
	constexpr float TileGap = 0.2f;
	constexpr float TileStride = TileSize + TileGap;
	constexpr float Pi = 3.14159265f;
	constexpr Vec3 BoardOrigin = { 0.5f, 0.0f, -0.5f };
	constexpr Vec3 FaceDownRotation = { 0.0f, 0.0f, 0.0f };
	constexpr Vec3 FaceUpRotation = { 0.0f, 0.0f, Pi };
	constexpr float TileLiftHeight = 0.5f;
	constexpr float TileLiftDuration = 0.15f;
	constexpr float TileFlipDuration = 0.25f;
	constexpr float PairReviewDuration = 0.75f;
	constexpr float MatchJiggleDuration = 0.35f;
	constexpr float MatchVanishDuration = 0.45f;
	constexpr float MatchJiggleDistance = 0.08f;
	constexpr float MatchVanishLift = 0.8f;
	constexpr std::size_t InvalidTileIndex = static_cast<std::size_t>(-1);

	using TileNameList = std::array<std::string, TileColumns * TileRows>;

	const TileNameList& TileNames()
	{
		static const TileNameList names = {
			"tile_dog_1",      "tile_dog_2",
			"tile_cat_1",      "tile_cat_2",
			"tile_bear_1",     "tile_bear_2",
			"tile_bird_1",     "tile_bird_2",
			"tile_capybara_1", "tile_capybara_2",
			"tile_frog_1",     "tile_frog_2",
			"tile_giraffe_1",  "tile_giraffe_2",
			"tile_lion_1",     "tile_lion_2",
			"tile_snake_1",    "tile_snake_2",
			"tile_tiger_1",    "tile_tiger_2",
		};

		return names;
	}
}

class Memory final : public IGame
{
public:
	virtual void OnInit(EngineServices& services) override
	{
		m_scene = services.scene;
		m_input = services.input;

		LogTileIds();
		PlaceTiles();
	}

	virtual void OnUpdate(const FrameTime& frameTime) override
	{
		const float dt = static_cast<float>(frameTime.dt);

		UpdateTileFlip(dt);
		UpdateMatchAnimations(dt);
		if (HasActiveFlip() || HasActiveMatchAnimation())
		{
			return;
		}

		if (m_waitingToResolvePair)
		{
			UpdatePairReview(dt);
			return;
		}

		if (m_input->MouseClicked(InputMouseButton::Left))
		{
			m_pendingTileClick = true;
			return;
		}

		if (!m_pendingTileClick)
		{
			return;
		}

		const EntityId tileId = m_scene->GetSelectedEntity();
		m_pendingTileClick = false;

		if (!CanFlip(tileId))
		{
			return;
		}

		StartTileFlip(tileId);
	}

	virtual void OnShutdown() override
	{
		// Shutdown logic here
	}

	std::string GetStartupSceneName() const override
	{
		return "memory"; // corresponds to assets/chess/chess.xml in your scheme
	}

private:
	struct Tile
	{
		EntityId id = InvalidEntity;
		std::size_t pairId = 0;
		Vec3 homePosition{};
		bool faceUp = false;
		bool matched = false;
	};

	using TileList = std::array<Tile, TileColumns * TileRows>;

	enum class FlipPhase
	{
		Idle,
		Lifting,
		Rotating,
		Dropping,
	};

	enum class FlipDirection
	{
		Up,
		Down,
	};

	struct TileFlip
	{
		std::size_t tileIndex = InvalidTileIndex;
		FlipPhase phase = FlipPhase::Idle;
		FlipDirection direction = FlipDirection::Up;
		float elapsed = 0.0f;
		Vec3 startPosition{};
		Vec3 targetPosition{};
		Vec3 startRotation{};
		Vec3 targetRotation{};
	};

	struct MatchAnimation
	{
		std::size_t tileIndex = InvalidTileIndex;
		float elapsed = 0.0f;
		Vec3 startPosition{};
		Vec3 vanishPosition{};
	};

	Vec3 TilePosition(std::size_t column, std::size_t row) const
	{
		return {
			BoardOrigin.x + static_cast<float>(column) * TileStride,
			BoardOrigin.y,
			BoardOrigin.z - static_cast<float>(row) * TileStride,
		};
	}

	void LogTileIds() const
	{
		for (const auto& name : TileNames())
		{
			const EntityId pieceId = m_scene->FindEntityByName(name);
			std::cout << name << ": " << pieceId << std::endl;
		}
	}

	TileList FindTiles() const
	{
		TileList tiles{};
		const auto& names = TileNames();

		for (std::size_t i = 0; i < names.size(); ++i)
		{
			tiles[i] = {
				.id = m_scene->FindEntityByName(names[i]),
				.pairId = i / 2,
			};
		}

		return tiles;
	}

	void ShuffleTiles(TileList& tiles)
	{
		std::shuffle(tiles.begin(), tiles.end(), m_randomEngine);
	}

	void PlaceTiles()
	{
		m_tiles = FindTiles();
		ShuffleTiles(m_tiles);
		m_revealedTileCount = 0;
		m_activeFlips = {};
		m_matchAnimations = {};
		m_waitingToResolvePair = false;
		m_pairReviewElapsed = 0.0f;
		m_pendingTileClick = false;

		for (std::size_t column = 0; column < TileColumns; ++column)
		{
			for (std::size_t row = 0; row < TileRows; ++row)
			{
				const auto tileIndex = column * TileRows + row;
				Tile& tile = m_tiles[tileIndex];

				tile.homePosition = TilePosition(column, row);
				tile.faceUp = false;
				tile.matched = false;

				m_scene->SetPosition(tile.id, tile.homePosition);
				m_scene->SetRotationEuler(tile.id, FaceDownRotation);
				m_scene->SetScale(tile.id, { 1.0f, 1.0f, 1.0f });
			}
		}
	}

	bool IsTile(EntityId entityId) const
	{
		return FindTileIndex(entityId) != InvalidTileIndex;
	}

	std::size_t FindTileIndex(EntityId entityId) const
	{
		for (std::size_t i = 0; i < m_tiles.size(); ++i)
		{
			if (m_tiles[i].id == entityId)
			{
				return i;
			}
		}

		return InvalidTileIndex;
	}

	bool CanFlip(EntityId tileId) const
	{
		const auto tileIndex = FindTileIndex(tileId);
		if (tileIndex == InvalidTileIndex || HasActiveFlip())
		{
			return false;
		}

		return !m_tiles[tileIndex].faceUp && !m_tiles[tileIndex].matched;
	}

	bool HasActiveFlip() const
	{
		for (const auto& flip : m_activeFlips)
		{
			if (flip.phase != FlipPhase::Idle)
			{
				return true;
			}
		}

		return false;
	}

	bool HasActiveMatchAnimation() const
	{
		for (const auto& animation : m_matchAnimations)
		{
			if (animation.tileIndex != InvalidTileIndex)
			{
				return true;
			}
		}

		return false;
	}

	void StartTileFlip(EntityId tileId)
	{
		StartFlipUp(FindTileIndex(tileId));
	}

	void StartFlipUp(std::size_t tileIndex)
	{
		Tile& tile = m_tiles[tileIndex];

		StartFlip({
			.tileIndex = tileIndex,
			.phase = FlipPhase::Lifting,
			.direction = FlipDirection::Up,
			.elapsed = 0.0f,
			.startPosition = tile.homePosition,
			.targetPosition = RaisedPosition(tile),
			.startRotation = FaceDownRotation,
			.targetRotation = FaceUpRotation,
		});
	}

	void StartFlipDown(std::size_t tileIndex)
	{
		Tile& tile = m_tiles[tileIndex];

		StartFlip({
			.tileIndex = tileIndex,
			.phase = FlipPhase::Rotating,
			.direction = FlipDirection::Down,
			.elapsed = 0.0f,
			.startPosition = RaisedPosition(tile),
			.targetPosition = tile.homePosition,
			.startRotation = FaceUpRotation,
			.targetRotation = FaceDownRotation,
		});
	}

	void StartFlip(TileFlip flip)
	{
		for (auto& activeFlip : m_activeFlips)
		{
			if (activeFlip.phase == FlipPhase::Idle)
			{
				activeFlip = flip;
				return;
			}
		}
	}

	void UpdateTileFlip(float dt)
	{
		for (auto& flip : m_activeFlips)
		{
			if (flip.phase == FlipPhase::Idle)
			{
				continue;
			}

			const bool completed = UpdateSingleFlip(flip, dt);
			if (!completed)
			{
				continue;
			}

			const std::size_t tileIndex = flip.tileIndex;
			const FlipDirection direction = flip.direction;
			flip = {};

			if (direction == FlipDirection::Up)
			{
				OnTileFlippedUp(tileIndex);
			}
			else
			{
				OnTileFlippedDown(tileIndex);
			}
		}
	}

	bool UpdateSingleFlip(TileFlip& flip, float dt)
	{
		flip.elapsed += dt;
		Tile& tile = m_tiles[flip.tileIndex];

		if (flip.phase == FlipPhase::Lifting)
		{
			const float t = EaseOutQuad(flip.elapsed / TileLiftDuration);
			m_scene->SetPosition(tile.id, Lerp(flip.startPosition, flip.targetPosition, t));

			if (flip.elapsed < TileLiftDuration)
			{
				return false;
			}

			m_scene->SetPosition(tile.id, flip.targetPosition);
			flip.phase = FlipPhase::Rotating;
			flip.elapsed = 0.0f;
			return false;
		}

		if (flip.phase == FlipPhase::Rotating)
		{
			const float t = EaseOutQuad(flip.elapsed / TileFlipDuration);
			m_scene->SetRotationEuler(tile.id, Lerp(flip.startRotation, flip.targetRotation, t));

			if (flip.elapsed < TileFlipDuration)
			{
				return false;
			}

			m_scene->SetRotationEuler(tile.id, flip.targetRotation);
			flip.elapsed = 0.0f;

			if (flip.direction == FlipDirection::Up)
			{
				return true;
			}

			flip.phase = FlipPhase::Dropping;
			return false;
		}

		const float t = EaseOutQuad(flip.elapsed / TileLiftDuration);
		m_scene->SetPosition(tile.id, Lerp(flip.startPosition, flip.targetPosition, t));

		if (flip.elapsed < TileLiftDuration)
		{
			return false;
		}

		m_scene->SetPosition(tile.id, flip.targetPosition);
		return true;
	}

	void OnTileFlippedUp(std::size_t tileIndex)
	{
		m_tiles[tileIndex].faceUp = true;
		m_revealedTiles[m_revealedTileCount++] = tileIndex;

		if (m_revealedTileCount == 2)
		{
			StartPairReview();
		}
	}

	void OnTileFlippedDown(std::size_t tileIndex)
	{
		m_tiles[tileIndex].faceUp = false;
	}

	void ResolveRevealedPair()
	{
		const std::size_t first = m_revealedTiles[0];
		const std::size_t second = m_revealedTiles[1];
		m_revealedTileCount = 0;

		if (m_tiles[first].pairId == m_tiles[second].pairId)
		{
			MatchTiles(first, second);
			return;
		}

		StartFlipDown(first);
		StartFlipDown(second);
	}

	void StartPairReview()
	{
		m_waitingToResolvePair = true;
		m_pairReviewElapsed = 0.0f;
	}

	void UpdatePairReview(float dt)
	{
		m_pairReviewElapsed += dt;
		if (m_pairReviewElapsed < PairReviewDuration)
		{
			return;
		}

		m_waitingToResolvePair = false;
		m_pairReviewElapsed = 0.0f;
		ResolveRevealedPair();
	}

	void MatchTiles(std::size_t first, std::size_t second)
	{
		m_tiles[first].matched = true;
		m_tiles[second].matched = true;

		StartMatchAnimation(first);
		StartMatchAnimation(second);
	}

	void StartMatchAnimation(std::size_t tileIndex)
	{
		const Tile& tile = m_tiles[tileIndex];
		const Vec3 startPosition = RaisedPosition(tile);

		for (auto& animation : m_matchAnimations)
		{
			if (animation.tileIndex == InvalidTileIndex)
			{
				animation = {
					.tileIndex = tileIndex,
					.elapsed = 0.0f,
					.startPosition = startPosition,
					.vanishPosition = { startPosition.x, startPosition.y + MatchVanishLift, startPosition.z },
				};
				return;
			}
		}
	}

	void UpdateMatchAnimations(float dt)
	{
		for (auto& animation : m_matchAnimations)
		{
			if (animation.tileIndex == InvalidTileIndex)
			{
				continue;
			}

			const bool completed = UpdateSingleMatchAnimation(animation, dt);
			if (completed)
			{
				HideTile(animation.tileIndex);
				animation = {};
			}
		}
	}

	bool UpdateSingleMatchAnimation(MatchAnimation& animation, float dt)
	{
		animation.elapsed += dt;
		Tile& tile = m_tiles[animation.tileIndex];

		if (animation.elapsed < MatchJiggleDuration)
		{
			const float t = animation.elapsed / MatchJiggleDuration;
			const float direction = (animation.tileIndex % 2 == 0) ? 1.0f : -1.0f;
			const float offset = std::sin(t * 6.0f * Pi) * MatchJiggleDistance * direction;

			m_scene->SetPosition(tile.id, { animation.startPosition.x + offset, animation.startPosition.y, animation.startPosition.z });
			return false;
		}

		const float vanishElapsed = animation.elapsed - MatchJiggleDuration;
		const float t = EaseOutQuad(vanishElapsed / MatchVanishDuration);
		const float scale = 1.0f - t;

		m_scene->SetPosition(tile.id, Lerp(animation.startPosition, animation.vanishPosition, t));
		m_scene->SetScale(tile.id, { scale, scale, scale });

		return vanishElapsed >= MatchVanishDuration;
	}

	void HideTile(std::size_t tileIndex)
	{
		Tile& tile = m_tiles[tileIndex];

		m_scene->SetPosition(tile.id, RaisedPosition(tile));
		m_scene->SetScale(tile.id, { 0.0f, 0.0f, 0.0f });
	}

	Vec3 RaisedPosition(const Tile& tile) const
	{
		return {
			tile.homePosition.x,
			tile.homePosition.y + TileLiftHeight,
			tile.homePosition.z,
		};
	}

	float Clamp01(float value) const
	{
		return std::clamp(value, 0.0f, 1.0f);
	}

	float EaseOutQuad(float t) const
	{
		t = Clamp01(t);
		return 1.0f - (1.0f - t) * (1.0f - t);
	}

	Vec3 Lerp(Vec3 from, Vec3 to, float t) const
	{
		t = Clamp01(t);
		return {
			from.x + (to.x - from.x) * t,
			from.y + (to.y - from.y) * t,
			from.z + (to.z - from.z) * t,
		};
	}

	SceneService* m_scene = nullptr;
	IInput* m_input = nullptr;
	TileList m_tiles{};
	std::array<TileFlip, 2> m_activeFlips{};
	std::array<MatchAnimation, 2> m_matchAnimations{};
	std::array<std::size_t, 2> m_revealedTiles{};
	std::size_t m_revealedTileCount = 0;
	bool m_waitingToResolvePair = false;
	float m_pairReviewElapsed = 0.0f;
	bool m_pendingTileClick = false;
	std::mt19937 m_randomEngine{ std::random_device{}() };
};

int main(int argc, char* argv[])
{
	Engine engine;
	engine.CreateRenderResources();

	Memory app;
	engine.Run(app);
	return 0;
}
