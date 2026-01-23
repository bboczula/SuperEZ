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

	virtual void OnUpdate(const FrameTime& frameTime) override
	{
		const float t = static_cast<float>(frameTime.time); // seconds (scaled)
		// If you want it to keep oscillating even when paused/slowmo, use frameTime.unscaledTime.

		auto earth = services.scene->FindEntityByName("Earth_Mesh");

		// Parameters
		const float amplitudeDeg = 180.0f;            // swing size: +/- 45°
		const float cyclesPerSec = 0.10f;            // 0.10 Hz => 10 seconds per full cycle
		const float omega = 2.0f * 3.14159265f * cyclesPerSec;

		// Baseline/origin yaw (choose one):
		// 1) Fixed baseline:
		const float baselineYaw = 0.0f;
		// 2) Or capture initial yaw once and keep it (recommended). See note below.

		Vec3 rot = services.scene->GetRotationEuler(earth);
		rot.y = baselineYaw + amplitudeDeg * std::sin(omega * t);
		services.scene->SetRotationEuler(earth, rot);

		// ...moon/mars as before (dt-based constant spin), or also oscillate them
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