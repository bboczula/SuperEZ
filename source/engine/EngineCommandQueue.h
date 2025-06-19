#pragma once

#include <string>
#include <queue>
#include <mutex>

enum class EngineCommandType
{
	None,
	Startup,
	LoadAssets,
	GameLoop,
	Exit
};

struct EngineCommand
{
	EngineCommandType type;
	std::string nextStateName;
};

class GlobalCommandQueue
{
public:
	static void Push(const EngineCommand& cmd);
	static std::queue<EngineCommand> Consume(); // flush & return all

private:
	static std::mutex mutex;
	static std::queue<EngineCommand> queue;
};
