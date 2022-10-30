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
#include "GameEngine/Contract.h"
#include <string>
#include <vector>

namespace Enigma::SceneGraph
{
    class BuildSceneGraph : public Frameworks::ICommand
    {
    public:
        BuildSceneGraph(const std::string& scene_graph_id, const std::vector<Engine::Contract>& contracts)
            : m_id(scene_graph_id), m_contracts(contracts) {}

        const std::string& GetSceneGraphId() { return m_id; }
        const std::vector<Engine::Contract>& GetContracts() { return m_contracts; }

    protected:
        std::string m_id;
        std::vector<Engine::Contract> m_contracts;
    };
}

#endif // _SCENE_GRAPH_COMMANDS_H
