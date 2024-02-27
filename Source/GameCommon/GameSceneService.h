/*********************************************************************
 * \file   GameSceneService.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   March 2023
 *********************************************************************/
#ifndef _GAME_SCENE_SERVICE_H
#define _GAME_SCENE_SERVICE_H

#include "Frameworks/SystemService.h"
#include "SceneGraph/SceneGraphRepository.h"
#include "SceneGraph/Node.h"
#include "SceneGraph/PortalManagementNode.h"
#include "SceneGraph/Spatial.h"
#include "SceneGraph/Culler.h"
#include "Frameworks/EventSubscriber.h"
#include "Frameworks/CommandSubscriber.h"

namespace Enigma::GameCommon
{
    using error = std::error_code;

    class GameCameraService;

    class GameSceneService : public Frameworks::ISystemService
    {
        DECLARE_EN_RTTI;
    public:
        GameSceneService(Frameworks::ServiceManager* mngr, const std::shared_ptr<SceneGraph::SceneGraphRepository>& scene_graph_repository,
            const std::shared_ptr<GameCameraService>& camera_service);
        GameSceneService(const GameSceneService&) = delete;
        GameSceneService(GameSceneService&&) = delete;
        virtual ~GameSceneService() override;
        GameSceneService& operator=(const GameSceneService&) = delete;
        GameSceneService& operator=(GameSceneService&&) = delete;

        Frameworks::ServiceResult onInit() override;
        Frameworks::ServiceResult onTick() override;
        Frameworks::ServiceResult onTerm() override;

        /** @name scene root */
        //@{
        void createRootScene(const SceneGraph::SpatialId& scene_root_id, const std::optional<std::string>& portal_managed_name);
        void destroyRootScene();
        const std::shared_ptr<SceneGraph::Node>& getSceneRoot() { return m_sceneRoot; };
        //@}

        const std::shared_ptr<SceneGraph::PortalManagementNode>& GetPortalManagementNode() { return m_portalMgtNode; };
        error attachOutsideZone(const std::shared_ptr<SceneGraph::PortalZoneNode>& node);

        std::shared_ptr<SceneGraph::Spatial> FindSpatialByName(const std::string& spatial_name);

        /** create culler */
        void CreateSceneCuller(const std::shared_ptr<SceneGraph::Camera>& camera);
        /** destroy culler */
        void DestroySceneCuller();
        /** get scene culler */
        SceneGraph::Culler* getSceneCuller() { return m_culler; };

    protected:
        void OnGameCameraCreated(const Frameworks::IEventPtr& e);
        void OnGameCameraUpdated(const Frameworks::IEventPtr& e);
        void DoAttachingSceneRootChild(const Frameworks::ICommandPtr& c);
        void DoAttachingNodeChild(const Frameworks::ICommandPtr& c);
        void DoDeletingSceneSpatial(const Frameworks::ICommandPtr& c);

    protected:
        std::weak_ptr<SceneGraph::SceneGraphRepository> m_sceneGraphRepository;
        std::weak_ptr<GameCameraService> m_cameraService;
        std::shared_ptr<SceneGraph::Node> m_sceneRoot;
        std::shared_ptr<SceneGraph::PortalManagementNode> m_portalMgtNode;
        SceneGraph::Culler* m_culler;

        Frameworks::EventSubscriberPtr m_onCameraCreated;
        Frameworks::EventSubscriberPtr m_onCameraUpdated;
        Frameworks::CommandSubscriberPtr m_doAttachingSceneRootChild;
        Frameworks::CommandSubscriberPtr m_doAttachingNodeChild;
        Frameworks::CommandSubscriberPtr m_doDeletingSceneSpatial;
    };
}

#endif // _GAME_SCENE_SERVICE_H
