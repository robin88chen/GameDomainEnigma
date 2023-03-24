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
        EngineInstallingPolicy() = default;
        virtual ~EngineInstallingPolicy() override = default;

        virtual void Install(Frameworks::ServiceManager* service_manager) override;
        virtual void Shutdown(Frameworks::ServiceManager* service_manager) override;
    };
}
#endif // _ENGINE_INSTALLING_POLICY_H
