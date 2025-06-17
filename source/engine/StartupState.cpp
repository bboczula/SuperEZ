#include "StartupState.h"
#include "Engine.h"
#include <Windows.h>

void StartupState::Enter(Engine& engine)
{
	OutputDebugString(L"Entering Startup State\n");
	engine.Initialize();
}

void StartupState::Exit(Engine& engine)
{
	OutputDebugString(L"Exiting Startup State\n");
}

void StartupState::Update(Engine& engine)
{
}
