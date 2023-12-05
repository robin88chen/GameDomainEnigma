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
#include "SpatialId.h"
#include <string>
#include <vector>

namespace Enigma::SceneGraph
{
    class Node;
    class LazyNode;
    class Spatial;

    class BuildSceneGraph : public Frameworks::ICommand
    {
    public:
        BuildSceneGraph(const std::string& scene_graph_id, const Engine::GenericDtoCollection& dtos)
            : m_id(scene_graph_id), m_dtos(dtos) {}

        const std::string& GetSceneGraphId() { return m_id; }
        const Engine::GenericDtoCollection& GetDtos() { return m_dtos; }

    protected:
        std::string m_id;
        Engine::GenericDtoCollection m_dtos;
    };

    class InstanceLazyNode : public Frameworks::ICommand
    {
    public:
        InstanceLazyNode(const std::shared_ptr<LazyNode>& node) : m_node(node) {}

        const std::shared_ptr<LazyNode>& GetNode() { return m_node; }

    protected:
        std::shared_ptr<LazyNode> m_node;
    };

    class InPlaceBuildSceneGraph : public Frameworks::ICommand
    {
    public:
        InPlaceBuildSceneGraph(const std::shared_ptr<Node>& owner_node, const Engine::GenericDtoCollection& dtos)
            : m_ownerNode(owner_node), m_dtos(dtos) {}

        const std::shared_ptr<Node>& GetOwnerNode() { return m_ownerNode; }
        const Engine::GenericDtoCollection& GetDtos() { return m_dtos; }

    protected:
        std::shared_ptr<Node> m_ownerNode;
        Engine::GenericDtoCollection m_dtos;
    };
    //--------------------------------------------------------------------------------
    using SpatialDtoFactory = std::function<Spatial* (const Engine::GenericDto& dto)>;

    class RegisterSpatialDtoFactory : public Frameworks::ICommand
    {
    public:
        RegisterSpatialDtoFactory(const std::string& rtti, const SpatialDtoFactory& factory)
            : m_rtti(rtti), m_factory(factory) {}

        const std::string& GetRtti() const { return m_rtti; }
        const SpatialDtoFactory& GetFactory() { return m_factory; }

    private:
        std::string m_rtti;
        SpatialDtoFactory m_factory;
    };
    class UnRegisterSpatialDtoFactory : public Frameworks::ICommand
    {
    public:
        UnRegisterSpatialDtoFactory(const std::string& rtti) : m_rtti(rtti) {}

        const std::string& GetRtti() const { return m_rtti; }

    private:
        std::string m_rtti;
    };
    class InvokeSpatialDtoFactory : public Frameworks::ICommand
    {
    public:
        InvokeSpatialDtoFactory(const Engine::GenericDto& dto) : m_dto(dto) {}

        const Engine::GenericDto& GetDto() { return m_dto; }

    private:
        Engine::GenericDto m_dto;
    };
    //--------------------------- Creator --------------------------------------
    class CreateNode : public Frameworks::IRequestCommand
    {
    public:
        CreateNode(const std::string& name, const Engine::FactoryDesc& factory_desc) : m_name(name), m_factory_desc(factory_desc) {}

        const std::string& name() { return m_name; }
        const Engine::FactoryDesc& factoryDesc() { return m_factory_desc; }

    protected:
        std::string m_name;
        Engine::FactoryDesc m_factory_desc;
    };
    class CreatePawn : public Frameworks::ICommand
    {
    public:
        CreatePawn(const SpatialId& id) : m_id(id) {}

        const SpatialId& id() { return m_id; }

    protected:
        SpatialId m_id;
    };
    class ConstitutePawn : public Frameworks::ICommand
    {
    public:
        ConstitutePawn(const SpatialId& id, const Engine::GenericDtoCollection& dtos) : m_id(id), m_dtos(dtos) {}

        const SpatialId& id() { return m_id; }
        const Engine::GenericDtoCollection& dtos() { return m_dtos; }

    protected:
        SpatialId m_id;
        Engine::GenericDtoCollection m_dtos;
    };
}

#endif // _SCENE_GRAPH_COMMANDS_H
