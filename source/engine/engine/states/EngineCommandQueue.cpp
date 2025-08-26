#include "EngineCommandQueue.h"

std::mutex GlobalCommandQueue::mutex;
std::queue<EngineCommand> GlobalCommandQueue::queue;

void GlobalCommandQueue::Push(const EngineCommand& cmd)
{
	std::lock_guard<std::mutex> lock(mutex);
	queue.push(cmd);
}

std::queue<EngineCommand> GlobalCommandQueue::Consume()
{
	std::lock_guard<std::mutex> lock(mutex);
	std::queue<EngineCommand> result;
	std::swap(result, queue);
	return result;
}
