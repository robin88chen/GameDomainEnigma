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

namespace Enigma::GameCommon
{
    class GameSceneService : public Frameworks::ISystemService
    {
        DECLARE_EN_RTTI;
    public:
        GameSceneService(Frameworks::ServiceManager* mngr, const std::shared_ptr<SceneGraph::SceneGraphRepository>& scene_graph_repository);
        GameSceneService(const GameSceneService&) = delete;
        GameSceneService(GameSceneService&&) = delete;
        virtual ~GameSceneService() override;
        GameSceneService& operator=(const GameSceneService&) = delete;
        GameSceneService& operator=(GameSceneService&&) = delete;

        Frameworks::ServiceResult OnInit() override;
        Frameworks::ServiceResult OnTick() override;
        Frameworks::ServiceResult OnTerm() override;

        /** @name scene root */
        //@{
        void CreateRootScene();
        void DestroyRootScene();
        const std::shared_ptr<SceneGraph::Node>& GetSceneRoot() { return m_sceneRoot; };
        //@}

        const std::shared_ptr<SceneGraph::PortalManagementNode>& GetPortalManagementNode() { return m_portalMgtNode; };

        std::shared_ptr<SceneGraph::Spatial> FindSpatialByName(const std::string& spatial_name);

        /** create culler */
        void CreateSceneCuller(const std::shared_ptr<SceneGraph::Camera>& camera);
        /** destroy culler */
        void DestroySceneCuller();
        /** get scene culler */
        SceneGraph::Culler* GetSceneCuller() { return m_culler; };

    protected:
        void OnGameCameraCreated(const Frameworks::IEventPtr& e);
        void OnGameCameraUpdated(const Frameworks::IEventPtr& e);

    protected:
        std::weak_ptr<SceneGraph::SceneGraphRepository> m_sceneGraphRepository;
        std::shared_ptr<SceneGraph::Node> m_sceneRoot;
        std::shared_ptr<SceneGraph::PortalManagementNode> m_portalMgtNode;
        SceneGraph::Culler* m_culler;

        Frameworks::EventSubscriberPtr m_onCameraCreated;
        Frameworks::EventSubscriberPtr m_onCameraUpdated;
    };
}

#endif // _GAME_SCENE_SERVICE_H
