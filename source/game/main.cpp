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
		auto earth = services.scene->FindEntityByName("Earth_Mesh");
		auto rot_earth = services.scene->GetRotationEuler(earth);
		rot_earth.y += 1.0f;
		services.scene->SetRotationEuler(earth, rot_earth);

		auto moon = services.scene->FindEntityByName("Moon_Mesh");
		auto rot_moon = services.scene->GetRotationEuler(moon);
		rot_moon.y += 0.25f;
		services.scene->SetScale(moon, { 0.5f, 0.5f, 0.5f });
		services.scene->SetRotationEuler(moon, rot_moon);

		auto mars = services.scene->FindEntityByName("Mars_Mesh");
		auto rot_mars = services.scene->GetRotationEuler(mars);
		rot_mars.y += 0.25f;
		services.scene->SetRotationEuler(mars, rot_mars);
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