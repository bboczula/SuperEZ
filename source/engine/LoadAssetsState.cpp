#include "LoadAssetsState.h"
#include "Engine.h"
#include <Windows.h>

void LoadAssetsState::Enter(Engine& engine)
{
	OutputDebugString(L"LoadAssetsState::Enter\n");
	engine.LoadSceneAssets();
}

void LoadAssetsState::Exit(Engine& engine)
{
	OutputDebugString(L"LoadAssetsState::Exit\n");
}

void LoadAssetsState::Update(Engine& engine)
{
}
