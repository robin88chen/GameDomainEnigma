/********************************************************************
 * \file   ControllerEvents.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   June 2022
 *********************************************************************/
#ifndef CONTROLLER_EVENTS_H
#define CONTROLLER_EVENTS_H

#include "Frameworks/Event.h"

namespace Enigma::Controllers
{
    class FrameworksInstalled : public Frameworks::IEvent
    {
    };
    class RenderEngineInstalled : public Frameworks::IEvent
    {
    };
}


#endif // CONTROLLER_EVENTS_H
