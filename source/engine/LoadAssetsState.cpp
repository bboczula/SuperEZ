#include "LoadAssetsState.h"
#include "Engine.h"
#include <Windows.h>
#include "EngineCommandQueue.h"

void LoadAssetsState::Enter(Engine& engine)
{
	OutputDebugString(L"LoadAssetsState::Enter\n");
	engine.LoadSceneAssets();

	// Transition to the next state after loading assets
	GlobalCommandQueue::Push(EngineCommand{ EngineCommandType::GameLoop });
}

void LoadAssetsState::Exit(Engine& engine)
{
	OutputDebugString(L"LoadAssetsState::Exit\n");
}

void LoadAssetsState::Update(Engine& engine)
{
}
