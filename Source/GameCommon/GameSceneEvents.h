/*********************************************************************
 * \file   GameSceneEvents.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   March 2023
 *********************************************************************/
#ifndef _GAME_SCENE_EVENTS_H
#define _GAME_SCENE_EVENTS_H

#include "Frameworks/Event.h"
#include "SceneGraph/Node.h"
#include "SceneGraph/PortalManagementNode.h"

namespace Enigma::GameCommon
{
    class SceneRootCreated : public Frameworks::IEvent
    {
    public:
        SceneRootCreated(const std::shared_ptr<SceneGraph::Node>& scene_root) : m_sceneRoot(scene_root) {}

        std::shared_ptr<SceneGraph::Node> GetSceneRoot() const { return m_sceneRoot.lock(); }

    protected:
        std::weak_ptr<SceneGraph::Node> m_sceneRoot;
    };
    class PortalManagementNodeCreated : public Frameworks::IEvent
    {
    public:
        PortalManagementNodeCreated(const std::shared_ptr<SceneGraph::PortalManagementNode>& management_node) : m_managementNode(management_node) {}

        std::shared_ptr<SceneGraph::PortalManagementNode> GetManagementNode() const { return m_managementNode.lock(); }

    protected:
        std::weak_ptr<SceneGraph::PortalManagementNode> m_managementNode;
    };
}

#endif // _GAME_SCENE_EVENTS_H
