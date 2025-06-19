#include "ExitState.h"
#include "Engine.h"
#include <Windows.h>
#include "EngineCommandQueue.h"

void ExitState::Enter(Engine& engine)
{
	OutputDebugString(L"Entering Exit State\n");
	exit(0); // Terminate the application
}

void ExitState::Exit(Engine& engine)
{
	OutputDebugString(L"Exiting Startup State\n");
}

void ExitState::Update(Engine& engine)
{
}
