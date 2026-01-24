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

		Coordinator* coordinator = services.scene->GetCoordinator();
		auto& transform = coordinator->GetComponent<TransformComponent>(7);

		basePosition[0] = transform.position[0];
		basePosition[1] = transform.position[1];
		basePosition[2] = transform.position[2];
	}

	virtual void OnUpdate(const FrameTime& frameTime) override
	{
		const float t = static_cast<float>(frameTime.time);

		// 1. Find the Entity ID
		// Note: You need to implement FindEntityByName in SceneService, 
		// OR just use ID 2 temporarily since we saw in the logs Earth is Entity 2.
		Entity moon = 6;
		Entity earth = 7;

		// 2. Get the Data Component
		Coordinator* coordinator = services.scene->GetCoordinator();
		auto& transform = coordinator->GetComponent<TransformComponent>(earth);

		// 3. Modify the Data directly
		const float amplitudeDeg = 1.0f;
		const float cyclesPerSec = 0.5f;
		const float omega = 2.0f * 3.14159265f * cyclesPerSec;

		static float phase = 0.0f;

		phase += omega * t;
		float yOffset = amplitudeDeg * std::sin(phase);

		transform.position[1] = basePosition[1] + 0.3f * std::sin(omega * t * 2.0f);

		// Handle the moon
		auto& tr = coordinator->GetComponent<TransformComponent>(moon);
	}



	virtual void OnShutdown() override
	{
		// Shutdown logic here
	}

	std::string GetStartupSceneName() const override
	{
		return "platform"; // corresponds to assets/chess/chess.xml in your scheme
	}
private:
	EngineServices services;
	float basePosition[3];
};

int main(int argc, char *argv[])
{
	Engine engine;
	engine.CreateRenderResources();

	ChessApp app;
	engine.Run(app);
	return 0;
}