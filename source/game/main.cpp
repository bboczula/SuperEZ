#include "../engine/engine/Engine.h"
#include "../engine/engine/IGame.h"

class ChessApp final : public IGame
{
public:
	std::string GetStartupSceneName() const override
	{
		return "sponza"; // corresponds to assets/chess/chess.xml in your scheme
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