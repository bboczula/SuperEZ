#pragma once

#include <string>
#include <queue>
#include <mutex>
#include "EngineCommand.h"

class GlobalCommandQueue
{
public:
	static void Push(const EngineCommand& cmd);
	static std::queue<EngineCommand> Consume(); // flush & return all

private:
	static std::mutex mutex;
	static std::queue<EngineCommand> queue;
};
