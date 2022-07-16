/*********************************************************************
 * \file   Timer.h
 * \brief  用 std chrono 建立 timer
 * 
 * \author Lancelot 'Robin' Chen
 * \date   July 2022
 *********************************************************************/
#ifndef _EN_TIMER_H
#define _EN_TIMER_H

#include <chrono>

namespace Enigma::Frameworks
{
    /** Timer Class */
    class Timer
    {
    public:
        Timer();
        virtual ~Timer();

        void Reset();  ///< 所有抓到的時間，從此開始重新計算
        void Update(); ///< update timer, 計算elapse, total time
        float GetElapseTime();  ///< 每個階段elapse time
        float GetTotalTime();  ///< 取得從上次reset算起總共經過的時間(pause到resume的時間不計)

        void Pause();  ///< timer pause, 紀錄此時的時間(留待resume的時候計算)
        void Resume();  ///< resume time, 將與pause time的時間差拿來修正last resume time

        /// scale >1, speed up; <1, slow down, =0, stop; <0, reverse 
        void SetScale(float scale);
        float GetScale() { return m_scale; };

        void SetFrameStep(bool force_step, float frame_step);
        float GetFrameStep() { return m_frameStep; };
        bool IsStepped() { return m_isForceStepped; };

        /// @return 系統啟動後的時間, 低解析度
        static double GetClockTime();

    protected:
        bool m_isTimerStopped;
        bool m_isForceStepped;

        std::chrono::high_resolution_clock::time_point m_lastTickedTime;
        std::chrono::high_resolution_clock::time_point m_pauseTime; ///< pause 時所抓到的時間
        std::chrono::high_resolution_clock::time_point m_currentTime;
        std::chrono::high_resolution_clock::time_point m_baseTime; ///< total time的計算基準,

        std::chrono::duration<float> m_currElapseTime;
        std::chrono::duration<float> m_totalTime;
        float m_scale;
        float m_frameStep;
    };
};

#endif // !_EN_TIMER_H
