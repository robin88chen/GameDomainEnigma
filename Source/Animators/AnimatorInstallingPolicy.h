/*********************************************************************
 * \file   AnimatorInstallingPolicy.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   March 2023
 *********************************************************************/
#ifndef ANIMATOR_INSTALLING_POLICY_H
#define ANIMATOR_INSTALLING_POLICY_H

#include "GameEngine/InstallingPolicy.h"

namespace Enigma::Animators
{
    class AnimatorInstallingPolicy : public Engine::InstallingPolicy
    {
    public:
        AnimatorInstallingPolicy() = default;
        virtual ~AnimatorInstallingPolicy() override = default;

        virtual void Install(Frameworks::ServiceManager* service_manager) override;
        virtual void Shutdown(Frameworks::ServiceManager* service_manager) override;
    };
}

#endif // ANIMATOR_INSTALLING_POLICY_H
