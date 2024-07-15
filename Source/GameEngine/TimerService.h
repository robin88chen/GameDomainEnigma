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
        TimerService(TimerService&&) = delete;
        virtual ~TimerService() override;
        TimerService& operator=(const TimerService&) = delete;
        TimerService& operator=(TimerService&&) = delete;

        virtual Frameworks::ServiceResult onInit() override;
        virtual Frameworks::ServiceResult onTick() override;
        virtual Frameworks::ServiceResult onTerm() override;

        const std::unique_ptr<Frameworks::Timer>& getGameTimer() { return m_gameTimer; };
        const std::unique_ptr<Frameworks::Timer>& getRealLifeTimer() { return m_realLifeTimer; };

        /// speed up : scale > 1.0, slow down : scale < 1.0, pause : scale = 0.0
        void setGameTimerScale(float scale);
        /// time step ( in seconds ) for every frame
        void setGameTimerStep(bool enable, float step);

        void pauseGameTimer();
        void resumeGameTimer();

        /// reverse timer
        void reverseGameTimer();

    protected:
        std::unique_ptr<Frameworks::Timer> m_gameTimer;
        std::unique_ptr<Frameworks::Timer> m_realLifeTimer;
    };
};

#endif // _TIMER_SERVICE_H
