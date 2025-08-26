#include "UnloadAssetsState.h"

#include <Windows.h>

#include "../Engine.h"

void UnloadAssetsState::Enter(Engine& engine)
{
	OutputDebugString(L"Entering Unload Assets State\n");
	engine.UnloadSceneAssets();
}

void UnloadAssetsState::Exit(Engine& engine)
{
}

void UnloadAssetsState::Update(Engine& engine)
{
}