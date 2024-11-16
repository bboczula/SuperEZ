#include "../engine/Engine.h"

int main(int argc, char *argv[])
{
	OutputDebugString(L"Main\n");
	Engine engine;
	engine.Initialize();
	// Add Game Item
	// - in this case a triangle or couple of them
	engine.CreateRenderResources();
	engine.Run();
	return 0;
}