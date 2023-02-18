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
#include <string>
#include <vector>

namespace Enigma::SceneGraph
{
    class LazyNode;
    class BuildSceneGraph : public Frameworks::ICommand
    {
    public:
        BuildSceneGraph(const std::string& scene_graph_id, const std::vector<Engine::GenericDto>& dtos)
            : m_id(scene_graph_id), m_dtos(dtos) {}

        const std::string& GetSceneGraphId() { return m_id; }
        const std::vector<Engine::GenericDto>& GetDtos() { return m_dtos; }

    protected:
        std::string m_id;
        std::vector<Engine::GenericDto> m_dtos;
    };

    class InstanceLazyNode : public Frameworks::ICommand
    {
    public:
        InstanceLazyNode(const std::shared_ptr<LazyNode>& node) : m_node(node) {}

        const std::shared_ptr<LazyNode>& GetNode() { return m_node; }

    protected:
        std::shared_ptr<LazyNode> m_node;
    };
}

#endif // _SCENE_GRAPH_COMMANDS_H
