#include "../engine/Engine.h"

int main(int argc, char *argv[])
{
	OutputDebugString(L"Main\n");
	Engine engine;
	engine.CreateRenderResources();
	engine.Run();
	return 0;
}