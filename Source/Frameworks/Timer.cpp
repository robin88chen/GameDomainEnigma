#include "Timer.h"

using hclock = std::chrono::high_resolution_clock;
using duration = std::chrono::duration<float>;

using namespace Enigma::Frameworks;

Timer::Timer() : m_scale(1.0f), m_frameStep(0.0f), m_isForceStepped(false), m_isTimerStopped(true), m_totalTime(duration(0.0f)), m_currElapseTime(duration(0.0f))
{
       Reset();
}

Timer::~Timer()
{
}

void Timer::Reset()
{
    m_lastTickedTime = hclock::now();
    m_currentTime = m_lastTickedTime;
    m_baseTime = m_lastTickedTime;
    m_pauseTime = m_lastTickedTime;

    m_currElapseTime = duration(0.0f);
    m_totalTime = duration(0.0f);
    m_scale = 1.0f;
    m_frameStep = 0.0f;
    m_isForceStepped = false;
    m_isTimerStopped = false;
}

void Timer::Update()
{
    m_lastTickedTime = m_currentTime;

    duration nonScaleElapse;
    if (m_isTimerStopped)  // 時間停止，用pause的時間來算
    {
        m_currentTime = m_pauseTime;
    }
    else
    {
        m_currentTime = hclock::now();
    }
    nonScaleElapse = duration(m_currentTime - m_lastTickedTime);

    if (m_isForceStepped)
    {
        nonScaleElapse = duration(m_frameStep);
    }
    m_currElapseTime = duration(m_scale * nonScaleElapse);
    m_totalTime += m_currElapseTime;
}

float Timer::GetElapseTime()
{
    return m_currElapseTime.count();
}

float Timer::GetTotalTime()
{
    return m_totalTime.count();
}

void Timer::Pause()
{
    if (m_isTimerStopped) return;  // already stop

    m_pauseTime = hclock::now();
    m_isTimerStopped = true;
}

void Timer::Resume()
{
    if (!m_isTimerStopped) return; // timer is running

    hclock::time_point tp_now = hclock::now();
    m_baseTime += (tp_now - m_pauseTime);
    m_lastTickedTime = tp_now;
    m_currentTime = tp_now;
    m_isTimerStopped = false;
}

void Timer::SetScale(float scale)
{
    m_scale = scale;
}

void Timer::SetFrameStep(bool force_step, float frame_step)
{
    m_isForceStepped = force_step;
    m_frameStep = frame_step;
}

double Timer::GetClockTime()
{
    std::chrono::system_clock::time_point t = std::chrono::system_clock::now();
    std::chrono::duration<double> diff = t.time_since_epoch();
    return diff.count();
}
