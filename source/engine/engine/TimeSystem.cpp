#include "TimeSystem.h"

void TimeSystem::Reset()
{
      m_lastTick = Clock::now();
      m_initialized = true;

      m_last = FrameTime{};
      m_last.timeScale = m_timeScale;
}

FrameTime TimeSystem::Tick()
{
      if (!m_initialized)
            Reset();

      const auto now = Clock::now();
      const std::chrono::duration<double> delta = now - m_lastTick;
      m_lastTick = now;

      // Raw wall-clock dt in seconds
      double rawDt = delta.count();

      // Guard against negative or nonsensical values (shouldn't happen with steady_clock, but be safe)
      if (rawDt < 0.0)
            rawDt = 0.0;

      // Clamp to avoid huge simulation jumps
      const double clampedDt = (m_maxDt > 0.0) ? std::min(rawDt, m_maxDt) : rawDt;

      // Apply scaling (pause/slowmo)
      const double scaledDt = clampedDt * m_timeScale;

      FrameTime ft;
      ft.unscaledDt = clampedDt;
      ft.dt = scaledDt;

      ft.unscaledTime = m_last.unscaledTime + clampedDt;
      ft.time = m_last.time + scaledDt;

      ft.frameIndex = m_last.frameIndex + 1;
      ft.timeScale = m_timeScale;

      m_last = ft;
      return ft;
}
