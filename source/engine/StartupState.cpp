#include "StartupState.h"
#include "Engine.h"
#include <Windows.h>
#include "EngineCommandQueue.h"

void StartupState::Enter(Engine& engine)
{
	OutputDebugString(L"Entering Startup State\n");
	engine.Initialize();

	// Transition to the next state
	GlobalCommandQueue::Push(EngineCommand{ EngineCommandType::LoadAssets });
}

void StartupState::Exit(Engine& engine)
{
	OutputDebugString(L"Exiting Startup State\n");
}

void StartupState::Update(Engine& engine)
{
}
