#pragma once
#include <cstdint>

struct FrameTime
{
	// Wall-clock frame delta (raw), seconds
	double unscaledDt = 0.0;

	// Simulation frame delta (after timeScale + clamp), seconds
	double dt = 0.0;

	// Accumulated wall-clock time since start, seconds
	double unscaledTime = 0.0;

	// Accumulated simulation time since start, seconds
	double time = 0.0;

	// Monotonic frame counter
	std::uint64_t frameIndex = 0;

	// Time scaling (0 = pause, 1 = realtime)
	double timeScale = 1.0;
};
