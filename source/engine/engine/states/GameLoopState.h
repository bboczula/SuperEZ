#pragma once

#include "IEngineState.h"

class GameLoopState : public IEngineState
{
public:
	GameLoopState() = default;
	void Enter(Engine& engine) override;
	void Exit(Engine& engine) override;
	void Update(Engine& engine) override;
};