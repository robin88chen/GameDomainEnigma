/*********************************************************************
 * \file   EngineInstallingPolicy.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   March 2023
 *********************************************************************/
#ifndef _ENGINE_INSTALLING_POLICY_H
#define _ENGINE_INSTALLING_POLICY_H

#include "InstallingPolicy.h"

namespace Enigma::Engine
{
    class EngineInstallingPolicy : public InstallingPolicy
    {
    public:
        EngineInstallingPolicy() {};

        virtual error install(Frameworks::ServiceManager* service_manager) override;
        virtual error shutdown(Frameworks::ServiceManager* service_manager) override;
    };
}
#endif // _ENGINE_INSTALLING_POLICY_H
