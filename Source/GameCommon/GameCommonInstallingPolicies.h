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
#include "SceneGraph/SceneGraphPersistenceLevel.h"

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
        GameSceneInstallingPolicy(const SceneGraph::SpatialId& scene_root_id, SceneGraph::PersistenceLevel persistence_level, const std::optional<SceneGraph::SpatialId>& portal_management_node_id)
            : m_sceneRootId(scene_root_id), m_persistenceLevel(persistence_level), m_portalManagementNodeId(portal_management_node_id) {}

        virtual error install(Frameworks::ServiceManager* service_manager) override;
        virtual error shutdown(Frameworks::ServiceManager* service_manager) override;

    protected:
        SceneGraph::SpatialId m_sceneRootId;
        SceneGraph::PersistenceLevel m_persistenceLevel;
        std::optional<SceneGraph::SpatialId> m_portalManagementNodeId;
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
