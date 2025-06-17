#pragma once

#include "IEngineState.h"

class LoadAssetsState : public IEngineState
{
public:
	LoadAssetsState() = default;
	void Enter(Engine& engine) override;
	void Exit(Engine& engine) override;
	void Update(Engine& engine) override;
};