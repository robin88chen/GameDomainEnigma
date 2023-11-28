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
    using error = std::error_code;
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
    //-----------------------------------------------------------------------------------
    class SceneRootChildAttached : public Frameworks::IEvent
    {
    public:
        SceneRootChildAttached(const std::shared_ptr<SceneGraph::Spatial>& child) : m_child(child) {}

        std::shared_ptr<SceneGraph::Spatial> GetChild() const { return m_child.lock(); }
    protected:
        std::weak_ptr<SceneGraph::Spatial> m_child;
    };
    class AttachSceneRootChildFailed : public Frameworks::IEvent
    {
    public:
        AttachSceneRootChildFailed(const std::string& child_name, error er) : m_childName(child_name), m_error(er) {}

        const std::string& GetChildName() const { return m_childName; }
        error GetError() const { return m_error; }

    protected:
        std::string m_childName;
        error m_error;
    };
    class SceneNodeChildAttached : public Frameworks::IEvent
    {
    public:
        SceneNodeChildAttached(const std::string& node_name, const std::shared_ptr<SceneGraph::Spatial>& child) : m_nodeName(node_name), m_child(child) {}

        const std::string& GetNodeName() const { return m_nodeName; }
        std::shared_ptr<SceneGraph::Spatial> GetChild() const { return m_child.lock(); }

    protected:
        std::string m_nodeName;
        std::weak_ptr<SceneGraph::Spatial> m_child;
    };
    class AttachSceneNodeChildFailed : public Frameworks::IEvent
    {
    public:
        AttachSceneNodeChildFailed(const std::string& node_name, const std::string& child_name, error er) : m_nodeName(node_name), m_childName(child_name), m_error(er) {}

        const std::string& GetNodeName() const { return m_nodeName; }
        const std::string& GetChildName() const { return m_childName; }
        error GetError() const { return m_error; }

    protected:
        std::string m_nodeName;
        std::string m_childName;
        error m_error;
    };
    //--------------------------------------------------------------------------
    class SceneSpatialDeleted : public Frameworks::IEvent
    {
    public:
        SceneSpatialDeleted(const std::string& spatial_name) : m_spatialName(spatial_name) {}

        const std::string& getSpatialName() const { return m_spatialName; }
    protected:
        std::string m_spatialName;
    };
    class DeleteSceneSpatialFailed : public Frameworks::IEvent
    {
    public:
        DeleteSceneSpatialFailed(const std::string& spatial_name, error er) : m_spatialName(spatial_name), m_error(er) {}

        const std::string& getSpatialName() const { return m_spatialName; }
        error GetError() const { return m_error; }

    protected:
        std::string m_spatialName;
        error m_error;
    };
}

#endif // _GAME_SCENE_EVENTS_H
