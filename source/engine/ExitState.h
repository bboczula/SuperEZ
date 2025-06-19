#pragma once

#include "IEngineState.h"

class ExitState : public IEngineState
{
public:
	ExitState() = default;
	void Enter(Engine& engine) override;
	void Exit(Engine& engine) override;
	void Update(Engine& engine) override;
};