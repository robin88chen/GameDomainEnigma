/*********************************************************************
 * \file   AnimationClip.h
 * \brief  animation clip, value object, use object
 *
 * \author Lancelot 'Robin' Chen
 * \date   January 2023
 *********************************************************************/
#ifndef _ANIMATION_CLIP_H
#define _ANIMATION_CLIP_H

namespace Enigma::Animators
{
    class AnimationClip
    {
    public:
        enum class WarpMode
        {
            Loop = 0x00,
            Clamp = 0x01,
        };

        enum class GoingToStop : bool
        {
            False,
            True
        };
    public:
        AnimationClip();
        AnimationClip(float start_offset, float loop_time, WarpMode warp, unsigned int divide_index);
        /// return true if this clip is going to stop
        GoingToStop update(float elapse_time);

        void SyncPhase(const AnimationClip& clip);
        void SetPhaseTime(float phase_time) { m_currentPhaseTime = phase_time; };

        float GetStartOffset() const { return m_startOffset; };
        float GetLoopTime() const { return m_loopTime; };
        WarpMode GetWarpMode() const { return m_warpMode; };
        unsigned int GetDivideIndex() const { return m_divideIndex; };

        void reset() { m_currentPhaseTime = 0.0f; };
        float CurrentTimeValue() const { return m_currentPhaseTime + m_startOffset; };
        float RemainLoopTime() const { return m_loopTime - m_currentPhaseTime; };

    protected:
        float m_startOffset;
        float m_loopTime;
        float m_currentPhaseTime;
        WarpMode m_warpMode;
        unsigned int m_divideIndex;
    };
}

#endif // _ANIMATION_CLIP_H
