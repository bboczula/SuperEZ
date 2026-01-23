#pragma once
#include "FrameTime.h"
#include <chrono>
#include <algorithm>

class TimeSystem
{
public:
	TimeSystem() { Reset(); }

	// Call once at startup, or whenever you need to re-base timing (e.g., after device reset).
	void Reset();

	// Call once per frame; returns the frame timing package.
	FrameTime Tick();

	// Controls
	void SetTimeScale(double scale) { m_timeScale = (std::max)(0.0, scale); }
	double GetTimeScale() const { return m_timeScale; }

	// Clamp huge dt spikes (debugger break, window drag, hitch).
	// Typical: 0.1 (100 ms) or 0.05 (50 ms).
	void SetMaxDeltaSeconds(double maxDt) { m_maxDt = (std::max)(0.0, maxDt); }
	double GetMaxDeltaSeconds() const { return m_maxDt; }

	const FrameTime& GetLastFrameTime() const { return m_last; }

private:
	using Clock = std::chrono::steady_clock;

	Clock::time_point m_lastTick{};
	bool m_initialized = false;

	double m_timeScale = 1.0;
	double m_maxDt = 0.1;

	FrameTime m_last{};
};
