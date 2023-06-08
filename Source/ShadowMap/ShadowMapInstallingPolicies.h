/*********************************************************************
 * \file   ShadowMapInstallingPolicies.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   June 2023
 *********************************************************************/
#ifndef _SHADOW_MAP_INSTALLING_POLICIES_H
#define _SHADOW_MAP_INSTALLING_POLICIES_H

#include "GameEngine/InstallingPolicy.h"
#include "ShadowMapServiceConfiguration.h"

namespace Enigma::ShadowMap
{
    using error = std::error_code;

    class ShadowMapInstallingPolicy : public Engine::InstallingPolicy
    {
    public:
        ShadowMapInstallingPolicy(const std::string& renderer_name, const std::string& target_name, std::unique_ptr<ShadowMapServiceConfiguration> config)
            : m_rendererName(renderer_name), m_targetName(target_name), m_config(std::move(config)) {}

        virtual error Install(Frameworks::ServiceManager* service_manager) override;
        virtual error Shutdown(Frameworks::ServiceManager* service_manager) override;

    protected:
        std::string m_rendererName;
        std::string m_targetName;
        std::unique_ptr<ShadowMapServiceConfiguration> m_config;
    };
}

#endif // _SHADOW_MAP_INSTALLING_POLICIES_H
