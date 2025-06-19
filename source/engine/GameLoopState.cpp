#include "GameLoopState.h"
#include "Engine.h"
#include <Windows.h>

void GameLoopState::Enter(Engine& engine)
{
	OutputDebugStringA("GameLoopState::Enter\n");
}

void GameLoopState::Exit(Engine& engine)
{
	OutputDebugStringA("GameLoopState::Exit\n");
}

void GameLoopState::Update(Engine& engine)
{
	engine.ProcessSingleFrame();
}
