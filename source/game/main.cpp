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
		const float t = static_cast<float>(frameTime.time);

		// 1. Find the Entity ID
		// Note: You need to implement FindEntityByName in SceneService, 
		// OR just use ID 2 temporarily since we saw in the logs Earth is Entity 2.
		Entity earth = 2;

		// 2. Get the Data Component
		Coordinator* coordinator = services.scene->GetCoordinator();
		auto& transform = coordinator->GetComponent<TransformComponent>(earth);

		// 3. Modify the Data directly
		const float amplitudeDeg = 5.0f;
		const float cyclesPerSec = 0.10f;
		const float omega = 2.0f * 3.14159265f * cyclesPerSec;

		// We are writing straight to memory now!
		transform.rotation[1] = amplitudeDeg * std::sin(omega * t); // Rotate Y
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