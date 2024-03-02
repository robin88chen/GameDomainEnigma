/*********************************************************************
 * \file   GameSceneCommands.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   August 2023
 *********************************************************************/
#ifndef GAME_SCENE_COMMANDS_H
#define GAME_SCENE_COMMANDS_H

#include "Frameworks/Command.h"
#include "SceneGraph/Spatial.h"

namespace Enigma::GameCommon
{
    class AttachSceneRootChild : public Frameworks::ICommand
    {
    public:
        AttachSceneRootChild(const std::shared_ptr<SceneGraph::Spatial>& child, const MathLib::Matrix4& local_transform) : m_child(child), m_localTransform(local_transform) {}

        const std::shared_ptr<SceneGraph::Spatial>& child() const { return m_child; }
        const MathLib::Matrix4& localTransform() const { return m_localTransform; }

    protected:
        std::shared_ptr<SceneGraph::Spatial> m_child;
        MathLib::Matrix4 m_localTransform;
    };
    class AttachNodeChild : public Frameworks::ICommand
    {
    public:
        AttachNodeChild(const SceneGraph::SpatialId& node_id, const std::shared_ptr<SceneGraph::Spatial>& child, const MathLib::Matrix4& local_transform) : m_nodeId(node_id), m_child(child), m_localTransform(local_transform) {}

        const SceneGraph::SpatialId& nodeId() const { return m_nodeId; }
        const std::shared_ptr<SceneGraph::Spatial>& child() const { return m_child; }
        const MathLib::Matrix4& localTransform() const { return m_localTransform; }

    protected:
        SceneGraph::SpatialId m_nodeId;
        std::shared_ptr<SceneGraph::Spatial> m_child;
        MathLib::Matrix4 m_localTransform;
    };
    class DeleteSceneSpatial : public Frameworks::ICommand
    {
    public:
        DeleteSceneSpatial(const SceneGraph::SpatialId& id) : m_id(id) {}

        const SceneGraph::SpatialId& id() const { return m_id; }

    protected:
        SceneGraph::SpatialId m_id;
    };
}

#endif // GAME_SCENE_COMMANDS_H
