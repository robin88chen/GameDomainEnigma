/*********************************************************************
 * \file   GameCommonInstallingPolicies.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   March 2023
 *********************************************************************/
#ifndef _GAME_COMMON_INSTALLING_POLICIES_H
#define _GAME_COMMON_INSTALLING_POLICIES_H

#include "GameEngine/InstallingPolicy.h"
#include "SceneGraph/CameraFrustumDtos.h"

namespace Enigma::GameCommon
{
    using error = std::error_code;

    class GameCameraInstallingPolicy : public Engine::InstallingPolicy
    {
    public:
        GameCameraInstallingPolicy(const SceneGraph::CameraDto& dto) : m_cameraDto(dto) {}

        virtual error Install(Frameworks::ServiceManager* service_manager) override;
        virtual error Shutdown(Frameworks::ServiceManager* service_manager) override;

    protected:
        SceneGraph::CameraDto m_cameraDto;
    };
    class GameLightInstallingPolicy : public Engine::InstallingPolicy
    {
    public:
        GameLightInstallingPolicy() = default;

        virtual error Install(Frameworks::ServiceManager* service_manager) override;
        virtual error Shutdown(Frameworks::ServiceManager* service_manager) override;
    };
    class GameSceneInstallingPolicy : public Engine::InstallingPolicy
    {
    public:
        GameSceneInstallingPolicy(const std::string& scene_root_name, const std::optional<std::string>& portal_managed_name)
            : m_sceneRootName(scene_root_name), m_portalManagedName(portal_managed_name) {}

        virtual error Install(Frameworks::ServiceManager* service_manager) override;
        virtual error Shutdown(Frameworks::ServiceManager* service_manager) override;

    protected:
        std::string m_sceneRootName;
        std::optional<std::string> m_portalManagedName;
    };
    class AnimatedPawnInstallingPolicy : public Engine::InstallingPolicy
    {
    public:
        AnimatedPawnInstallingPolicy() = default;

        virtual error Install(Frameworks::ServiceManager* service_manager) override;
        virtual error Shutdown(Frameworks::ServiceManager* service_manager) override;
    };
}

#endif // _GAME_COMMON_INSTALLING_POLICIES_H
