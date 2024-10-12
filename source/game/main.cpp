#include "../engine/Engine.h"

int main(int argc, char *argv[])
{
	OutputDebugString(L"Main\n");
	Engine engine;
	engine.Initialize();
	engine.Run();
	return 0;
}