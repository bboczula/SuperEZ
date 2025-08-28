#include "../engine/engine/Engine.h"

int main(int argc, char *argv[])
{
	Engine engine;
	engine.CreateRenderResources();
	engine.Run();
	return 0;
}