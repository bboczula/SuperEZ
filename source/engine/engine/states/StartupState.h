#pragma once

#include "IEngineState.h"

class StartupState : public IEngineState
{
public:
	StartupState() = default;
	void Enter(Engine& engine) override;
	void Exit(Engine& engine) override;
	void Update(Engine& engine) override;
};