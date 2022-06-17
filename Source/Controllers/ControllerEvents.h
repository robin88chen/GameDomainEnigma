/********************************************************************
 * \file   ApplicationEvents.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   June 2022
 *********************************************************************/
#ifndef APPLICATION_EVENTS_H
#define APPLICATION_EVENTS_H

#include "Frameworks/Event.h"

namespace Enigma::Controllers
{
    class FrameworksInstalled : public Frameworks::IEvent
    {
    };
}


#endif // APPLICATION_EVENTS_H
