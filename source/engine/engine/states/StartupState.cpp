#include "StartupState.h"
#include "../Engine.h"
#include <Windows.h>
#include "EngineCommandQueue.h"
#include "EngineCommandPayloads.h"

void StartupState::Enter(Engine& engine)
{
	OutputDebugString(L"Entering Startup State\n");
	engine.Initialize();

	// Provide the scene name via the command payload
	LoadAssetsPayload payload{ "sponza" };

	// Transition to the next state
	GlobalCommandQueue::Push(EngineCommand{ EngineCommandType::LoadAssets, payload });
}

void StartupState::Exit(Engine& engine)
{
	OutputDebugString(L"Exiting Startup State\n");
}

void StartupState::Update(Engine& engine)
{
}
