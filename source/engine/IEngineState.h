#pragma once

class Engine;

class IEngineState
{
public:
	virtual ~IEngineState() = default;
	virtual void Enter(Engine& engine) = 0;
	virtual void Exit(Engine& engine) = 0;
	virtual void Update(Engine& engine) = 0;
};