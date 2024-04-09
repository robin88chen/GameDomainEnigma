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
    class NodalSceneRootCreated : public Frameworks::IEvent
    {
    public:
        NodalSceneRootCreated(const std::shared_ptr<SceneGraph::Node>& scene_root) : m_root(scene_root) {}

        std::shared_ptr<SceneGraph::Node> root() const { return m_root.lock(); }

    protected:
        std::weak_ptr<SceneGraph::Node> m_root;
    };
    class CreateNodalSceneRootFailed : public Frameworks::IEvent
    {
    public:
        CreateNodalSceneRootFailed(const SceneGraph::SpatialId& root_id, std::error_code er) : m_rootId(root_id), m_error(er) {}

        const SceneGraph::SpatialId& rootId() const { return m_rootId; }
        std::error_code error() const { return m_error; }

    protected:
        SceneGraph::SpatialId m_rootId;
        std::error_code m_error;
    };
    class PortalManagementNodeCreated : public Frameworks::IEvent
    {
    public:
        PortalManagementNodeCreated(const std::shared_ptr<SceneGraph::PortalManagementNode>& management_node) : m_managementNode(management_node) {}

        std::shared_ptr<SceneGraph::PortalManagementNode> managementNode() const { return m_managementNode.lock(); }

    protected:
        std::weak_ptr<SceneGraph::PortalManagementNode> m_managementNode;
    };
    class CreatePortalManagementNodeFailed : public Frameworks::IEvent
    {
    public:
        CreatePortalManagementNodeFailed(const SceneGraph::SpatialId& portal_management_id, std::error_code er) : m_portalManagementId(portal_management_id), m_error(er) {}

        const SceneGraph::SpatialId& portalManagementId() const { return m_portalManagementId; }
        std::error_code error() const { return m_error; }

    protected:
        SceneGraph::SpatialId m_portalManagementId;
        std::error_code m_error;
    };
    //-----------------------------------------------------------------------------------
    class SceneRootChildAttached : public Frameworks::IEvent
    {
    public:
        SceneRootChildAttached(const std::shared_ptr<SceneGraph::Spatial>& child) : m_child(child) {}

        std::shared_ptr<SceneGraph::Spatial> child() const { return m_child.lock(); }
    protected:
        std::weak_ptr<SceneGraph::Spatial> m_child;
    };
    class AttachSceneRootChildFailed : public Frameworks::IEvent
    {
    public:
        AttachSceneRootChildFailed(const SceneGraph::SpatialId& child_id, std::error_code er) : m_childId(child_id), m_error(er) {}

        const SceneGraph::SpatialId& childId() const { return m_childId; }
        std::error_code error() const { return m_error; }

    protected:
        SceneGraph::SpatialId m_childId;
        std::error_code m_error;
    };
}

#endif // _GAME_SCENE_EVENTS_H
