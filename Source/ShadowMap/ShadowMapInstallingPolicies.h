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
#include "CascadeShadowMapServiceConfiguration.h"

namespace Enigma::ShadowMap
{
    using error = std::error_code;

    class ShadowMapInstallingPolicy : public Engine::InstallingPolicy
    {
    public:
        ShadowMapInstallingPolicy(const std::string& renderer_name, const std::string& target_name, const std::shared_ptr<ShadowMapServiceConfiguration>& config)
            : m_rendererName(renderer_name), m_targetName(target_name), m_config(config) {}

        virtual error install(Frameworks::ServiceManager* service_manager) override;
        virtual error shutdown(Frameworks::ServiceManager* service_manager) override;

    protected:
        std::string m_rendererName;
        std::string m_targetName;
        std::shared_ptr<ShadowMapServiceConfiguration> m_config;
    };
    class CascadeShadowMapInstallingPolicy : public Engine::InstallingPolicy
    {
    public:
        CascadeShadowMapInstallingPolicy(const std::string& renderer_name, const std::string& target_name, const std::shared_ptr<CascadeShadowMapServiceConfiguration>& config)
            : m_rendererName(renderer_name), m_targetName(target_name), m_config(config) {}

        virtual error install(Frameworks::ServiceManager* service_manager) override;
        virtual error shutdown(Frameworks::ServiceManager* service_manager) override;

    protected:
        std::string m_rendererName;
        std::string m_targetName;
        std::shared_ptr<CascadeShadowMapServiceConfiguration> m_config;
    };
}

#endif // _SHADOW_MAP_INSTALLING_POLICIES_H
