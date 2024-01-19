#include "AnimationClip.h"

using namespace Enigma::Renderables;

AnimationClip::AnimationClip()
{
    m_startOffset = 0.0f;
    m_loopTime = 1.0f;
    m_currentPhaseTime = 0.0f;
    m_warpMode = WarpMode::Loop;
    m_divideIndex = 0;
}

AnimationClip::AnimationClip(float start_offset, float loop_time, WarpMode warp, unsigned divide_index)
{
    m_startOffset = start_offset;
    m_loopTime = loop_time;
    m_currentPhaseTime = 0.0f;
    m_warpMode = warp;
    m_divideIndex = divide_index;
}

void AnimationClip::syncPhase(const AnimationClip& clip)
{
    m_currentPhaseTime = clip.m_currentPhaseTime;
    if (m_currentPhaseTime > m_loopTime) m_currentPhaseTime -= m_loopTime;
}

AnimationClip::GoingToStop AnimationClip::update(float elapse_time)
{
    GoingToStop going_to_stop = GoingToStop::False;
    m_currentPhaseTime += elapse_time;
    if (m_currentPhaseTime > m_loopTime)
    {
        if (m_warpMode == WarpMode::Clamp)
        {
            m_currentPhaseTime = m_loopTime;
            going_to_stop = GoingToStop::True;
        }
        else
        {
            int step = static_cast<int>(m_currentPhaseTime / m_loopTime);
            m_currentPhaseTime -= static_cast<float>(step) * m_loopTime;
        }
    }
    return going_to_stop;
}
