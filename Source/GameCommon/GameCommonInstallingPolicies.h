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
#include "SceneGraph/CameraDtos.h"

namespace Enigma::GameCommon
{
    using error = std::error_code;

    class GameCameraInstallingPolicy : public Engine::InstallingPolicy
    {
    public:
        GameCameraInstallingPolicy(const SceneGraph::SpatialId& id, const Engine::GenericDto& dto) : m_cameraId(id), m_cameraDto(dto) {}

        virtual error install(Frameworks::ServiceManager* service_manager) override;
        virtual error shutdown(Frameworks::ServiceManager* service_manager) override;

    protected:
        SceneGraph::SpatialId m_cameraId;
        Engine::GenericDto m_cameraDto;
    };
    class GameLightInstallingPolicy : public Engine::InstallingPolicy
    {
    public:
        GameLightInstallingPolicy() = default;

        virtual error install(Frameworks::ServiceManager* service_manager) override;
        virtual error shutdown(Frameworks::ServiceManager* service_manager) override;
    };
    class GameSceneInstallingPolicy : public Engine::InstallingPolicy
    {
    public:
        GameSceneInstallingPolicy() = default;

        virtual error install(Frameworks::ServiceManager* service_manager) override;
        virtual error shutdown(Frameworks::ServiceManager* service_manager) override;
    };
    class AnimatedPawnInstallingPolicy : public Engine::InstallingPolicy
    {
    public:
        AnimatedPawnInstallingPolicy() = default;

        virtual error install(Frameworks::ServiceManager* service_manager) override;
        virtual error shutdown(Frameworks::ServiceManager* service_manager) override;
    };
}

#endif // _GAME_COMMON_INSTALLING_POLICIES_H
