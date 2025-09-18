#pragma once

#include "IEngineState.h"

class UnloadAssetsState : public IEngineState
{
public:
	UnloadAssetsState() = default;
	void Enter(Engine& engine) override;
	void Exit(Engine& engine) override;
	void Update(Engine& engine) override;
};