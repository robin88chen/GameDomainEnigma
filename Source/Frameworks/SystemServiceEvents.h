/*********************************************************************
 * \file   SystemServiceEvents.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   June 2022
 *********************************************************************/
#ifndef SYSTEM_SERVICE_EVENTS_H
#define SYSTEM_SERVICE_EVENTS_H

#include "Event.h"

namespace Enigma::Frameworks
{
    class AllServiceInitialized : public IEvent
    {
    };
    //! 這個是廣播一個通知，即將要關掉所有服務。並非已發生的事件，不適合，也不適合做為 command
    //! 主要用在收到通知後，應用層先做一些相關處理，然後核心層可以安全關閉服務
    //! 如果核心服務以及應用層服務的依賴做好，是不是就不需要這個通知了??
    class ShutingdownAllService : public IEvent
    {
    };
}


#endif // SYSTEM_SERVICE_EVENTS_H
