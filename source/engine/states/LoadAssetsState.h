#pragma once

#include <string>

#include "IEngineState.h"

class LoadAssetsState : public IEngineState
{
public:
	LoadAssetsState(std::string sceneName);
	void Enter(Engine& engine) override;
	void Exit(Engine& engine) override;
	void Update(Engine& engine) override;
private:
	std::string sceneName; // Store the scene name for loading assets
};