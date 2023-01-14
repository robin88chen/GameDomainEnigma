/*********************************************************************
 * \file   TimerService.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   January 2023
 *********************************************************************/
#ifndef _TIMER_SERVICE_H
#define _TIMER_SERVICE_H

#include "Frameworks/SystemService.h"
#include "Frameworks/Timer.h"
#include <memory>

namespace Enigma::Engine
{
    class TimerService : public Frameworks::ISystemService
    {
        DECLARE_EN_RTTI;
    public:
        TimerService(Frameworks::ServiceManager* manager);
        TimerService(const TimerService&) = delete;
        virtual ~TimerService();
        TimerService& operator=(const TimerService&) = delete;

        virtual Frameworks::ServiceResult OnInit() override;
        virtual Frameworks::ServiceResult OnTick() override;
        virtual Frameworks::ServiceResult OnTerm() override;

        const std::unique_ptr<Frameworks::Timer>& GetGameTimer() { return m_gameTimer; };
        const std::unique_ptr<Frameworks::Timer>& GetRealLifeTimer() { return m_realLifeTimer; };

        /// speed up : scale > 1.0, slow down : scale < 1.0, pause : scale = 0.0 
        void SetGameTimerScale(float scale);
        /// time step ( in seconds ) for every frame
        void SetGameTimerStep(bool enable, float step);

        void PauseGameTimer();
        void ResumeGameTimer();

        /// reverse timer
        void ReverseGameTimer();

    protected:
        std::unique_ptr<Frameworks::Timer> m_gameTimer;
        std::unique_ptr<Frameworks::Timer> m_realLifeTimer;
    };
};

#endif // _TIMER_SERVICE_H
