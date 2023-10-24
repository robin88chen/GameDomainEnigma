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

        const std::shared_ptr<SceneGraph::Spatial>& GetChild() const { return m_child; }
        const MathLib::Matrix4& GetLocalTransform() const { return m_localTransform; }

    protected:
        std::shared_ptr<SceneGraph::Spatial> m_child;
        MathLib::Matrix4 m_localTransform;
    };
    class AttachNodeChild : public Frameworks::ICommand
    {
    public:
        AttachNodeChild(const std::string& node_name, const std::shared_ptr<SceneGraph::Spatial>& child, const MathLib::Matrix4& local_transform) : m_nodeName(node_name), m_child(child), m_localTransform(local_transform) {}

        const std::string& GetNodeName() const { return m_nodeName; }
        const std::shared_ptr<SceneGraph::Spatial>& GetChild() const { return m_child; }
        const MathLib::Matrix4& GetLocalTransform() const { return m_localTransform; }

    protected:
        std::string m_nodeName;
        std::shared_ptr<SceneGraph::Spatial> m_child;
        MathLib::Matrix4 m_localTransform;
    };
    class DeleteSceneSpatial : public Frameworks::ICommand
    {
    public:
        DeleteSceneSpatial(const std::string& name) : m_name(name) {}

        const std::string& getName() const { return m_name; }

    protected:
        std::string m_name;
    };
}

#endif // GAME_SCENE_COMMANDS_H
