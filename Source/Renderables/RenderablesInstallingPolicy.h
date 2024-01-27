/*********************************************************************
 * \file   RenderablesInstallingPolicy.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   January 2024
 *********************************************************************/
#ifndef RENDERABLES_INSTALLING_POLICY_H
#define RENDERABLES_INSTALLING_POLICY_H

#include "GameEngine/InstallingPolicy.h"

namespace Enigma::Renderables
{
    using error = std::error_code;

    class RenderablesInstallingPolicy : public Engine::InstallingPolicy
    {
    public:
        RenderablesInstallingPolicy() {}

        virtual error install(Frameworks::ServiceManager* service_manager) override;
        virtual error shutdown(Frameworks::ServiceManager* service_manager) override;
    };
}


#endif // RENDERABLES_INSTALLING_POLICY_H
