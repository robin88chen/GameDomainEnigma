/*********************************************************************
 * \file   SceneRendererInstallingPolicy.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   March 2023
 *********************************************************************/
#ifndef _SCENE_RENDERER_INSTALLING_POLICY_H
#define _SCENE_RENDERER_INSTALLING_POLICY_H

#include "GameEngine/InstallingPolicy.h"
#include "DeferredRendererServiceConfiguration.h"
#include "SceneRendererServiceConfiguration.h"

namespace Enigma::GameCommon
{
    using error = std::error_code;

    class SceneRendererInstallingPolicy : public Engine::InstallingPolicy
    {
    public:
        SceneRendererInstallingPolicy(const std::string& renderer_name, const std::string& target_name, const std::shared_ptr<SceneRendererServiceConfiguration>& config)
            : m_rendererName(renderer_name), m_targetName(target_name), m_config(config) {}

        virtual error Install(Frameworks::ServiceManager* service_manager) override;
        virtual error Shutdown(Frameworks::ServiceManager* service_manager) override;

    protected:
        std::string m_rendererName;
        std::string m_targetName;
        std::shared_ptr<SceneRendererServiceConfiguration> m_config;
    };

    class DeferredRendererInstallingPolicy : public Engine::InstallingPolicy
    {
    public:
        DeferredRendererInstallingPolicy(const std::string& renderer_name, const std::string& target_name, const std::shared_ptr<DeferredRendererServiceConfiguration>& config)
            : m_rendererName(renderer_name), m_targetName(target_name), m_config(config) {}
        virtual error Install(Frameworks::ServiceManager* service_manager) override;
        virtual error Shutdown(Frameworks::ServiceManager* service_manager) override;

    protected:
        std::string m_rendererName;
        std::string m_targetName;
        std::shared_ptr<DeferredRendererServiceConfiguration> m_config;
    };
}
#endif // _SCENE_RENDERER_INSTALLING_POLICY_H
