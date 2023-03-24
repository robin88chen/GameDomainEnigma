/*********************************************************************
 * \file   InstallingPolicy.h
 * \brief Installing Policy base interface
 *
 * \author Lancelot 'Robin' Chen
 * \date   March 2023
 *********************************************************************/
#ifndef _INSTALLING_POLICY_H
#define _INSTALLING_POLICY_H

#include "Frameworks/ServiceManager.h"

namespace Enigma::Engine
{
    class InstallingPolicy
    {
    public:
        InstallingPolicy() = default;
        virtual ~InstallingPolicy() = default;

        virtual void Install(Frameworks::ServiceManager* service_manager) = 0;
        virtual void Shutdown(Frameworks::ServiceManager* service_manager) = 0;
    };
}

#endif // _INSTALLING_POLICY_H
