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

namespace Enigma::GameCommon
{
    using error = std::error_code;

    class SceneRendererInstallingPolicy : public Engine::InstallingPolicy
    {
    public:
        SceneRendererInstallingPolicy(const std::string& renderer_name, const std::string& target_name, bool is_primary)
            : m_rendererName(renderer_name), m_targetName(target_name), m_isPrimary(is_primary) {}

        virtual error Install(Frameworks::ServiceManager* service_manager) override;
        virtual error Shutdown(Frameworks::ServiceManager* service_manager) override;

    protected:
        std::string m_rendererName;
        std::string m_targetName;
        bool m_isPrimary;
    };
}
#endif // _SCENE_RENDERER_INSTALLING_POLICY_H
