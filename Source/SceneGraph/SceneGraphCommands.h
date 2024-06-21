/*********************************************************************
 * \file   SceneGraphCommands.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef _SCENE_GRAPH_COMMANDS_H
#define _SCENE_GRAPH_COMMANDS_H

#include "Frameworks/Command.h"
#include "GameEngine/GenericDto.h"
#include "SceneGraphFactory.h"
#include "SpatialId.h"
#include "MathLib/Matrix4.h"
#include <string>
#include <vector>

namespace Enigma::SceneGraph
{
    class Node;
    class LazyNode;
    class Spatial;

    //--------------------------------------------------------------------------------
    class RegisterSpatialFactory : public Frameworks::ICommand
    {
    public:
        RegisterSpatialFactory(const std::string& rtti, const SpatialCreator& creator, const SpatialConstitutor& constitutor)
            : m_rtti(rtti), m_creator(creator), m_constitutor(constitutor) {}

        const std::string& rttiName() const { return m_rtti; }
        const SpatialCreator& creator() { return m_creator; }
        const SpatialConstitutor& constitutor() { return m_constitutor; }

    private:
        std::string m_rtti;
        SpatialCreator m_creator;
        SpatialConstitutor m_constitutor;
    };
    class UnregisterSpatialFactory : public Frameworks::ICommand
    {
    public:
        UnregisterSpatialFactory(const std::string& rtti) : m_rtti(rtti) {}

        const std::string& rttiName() const { return m_rtti; }

    private:
        std::string m_rtti;
    };
    class RegisterSpatialLightFactory : public Frameworks::ICommand
    {
    public:
        RegisterSpatialLightFactory(const std::string& rtti, const LightCreator& creator, const LightConstitutor& constitutor)
            : m_rtti(rtti), m_creator(creator), m_constitutor(constitutor) {}

        const std::string& rttiName() const { return m_rtti; }
        const LightCreator& creator() { return m_creator; }
        const LightConstitutor& constitutor() { return m_constitutor; }

    private:
        std::string m_rtti;
        LightCreator m_creator;
        LightConstitutor m_constitutor;
    };
    class UnregisterSpatialLightFactory : public Frameworks::ICommand
    {
    public:
        UnregisterSpatialLightFactory(const std::string& rtti) : m_rtti(rtti) {}

        const std::string& rttiName() const { return m_rtti; }

    private:
        std::string m_rtti;
    };
    //--------------------------- Repository operations ------------------------
    class PutSpatial : public Frameworks::ICommand
    {
    public:
        PutSpatial(const SpatialId& id, const std::shared_ptr<Spatial>& spatial) : m_id(id), m_spatial(spatial) {}

        const SpatialId& id() { return m_id; }
        const std::shared_ptr<Spatial>& spatial() { return m_spatial; }

    protected:
        SpatialId m_id;
        std::shared_ptr<Spatial> m_spatial;
    };
    class RemoveSpatial : public Frameworks::ICommand
    {
    public:
        RemoveSpatial(const SpatialId& id) : m_id(id) {}

        const SpatialId& id() { return m_id; }

    protected:
        SpatialId m_id;
    };
    class PutLaziedContent : public Frameworks::ICommand
    {
    public:
        PutLaziedContent(const SpatialId& id, const std::shared_ptr<LazyNode>& node) : m_id(id), m_node(node) {}

        const SpatialId& id() { return m_id; }
        const std::shared_ptr<LazyNode>& lazyNode() { return m_node; }

    protected:
        SpatialId m_id;
        std::shared_ptr<LazyNode> m_node;
    };
    class RemoveLaziedContent : public Frameworks::ICommand
    {
    public:
        RemoveLaziedContent(const SpatialId& id) : m_id(id) {}

        const SpatialId& id() { return m_id; }

    protected:
        SpatialId m_id;
    };
    class HydrateLazyNode : public Frameworks::ICommand
    {
    public:
        HydrateLazyNode(const SpatialId& id);

        const SpatialId& id() { return m_id; }

    protected:
        SpatialId m_id;
    };
    //--------------------------- Node operations ------------------------------
    class AttachNodeChild : public Frameworks::ICommand
    {
    public:
        AttachNodeChild(const SpatialId& node_id, const std::shared_ptr<Spatial>& child, const MathLib::Matrix4& local_transform) : m_nodeId(node_id), m_child(child), m_localTransform(local_transform) {}

        const SpatialId& nodeId() const { return m_nodeId; }
        const std::shared_ptr<Spatial>& child() const { return m_child; }
        const MathLib::Matrix4& localTransform() const { return m_localTransform; }

    protected:
        SpatialId m_nodeId;
        std::shared_ptr<Spatial> m_child;
        MathLib::Matrix4 m_localTransform;
    };
    class DetachNodeChild : public Frameworks::ICommand
    {
    public:
        DetachNodeChild(const SpatialId& node_id, const SpatialId& child_id) : m_nodeId(node_id), m_childId(child_id) {}

        const SpatialId& nodeId() const { return m_nodeId; }
        const SpatialId& childId() const { return m_childId; }

    protected:
        SpatialId m_nodeId;
        SpatialId m_childId;
    };
    //----------------------------------------------------------------
    class DeleteSceneSpatial : public Frameworks::ICommand
    {
    public:
        DeleteSceneSpatial(const SpatialId& id) : m_id(id) {}

        const SpatialId& id() const { return m_id; }

    protected:
        SpatialId m_id;
    };
}

#endif // _SCENE_GRAPH_COMMANDS_H
