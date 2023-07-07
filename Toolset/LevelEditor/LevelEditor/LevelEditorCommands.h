/*********************************************************************
 * \file   LevelEditorCommands.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   July 2023
 *********************************************************************/
#ifndef LEVEL_EDITOR_COMMANDS_H
#define LEVEL_EDITOR_COMMANDS_H

#include "Frameworks/Command.h"
#include "SceneGraph/Spatial.h"
#include <vector>

namespace LevelEditor
{
    class OutputMessage : public Enigma::Frameworks::ICommand
    {
    public:
        OutputMessage(const std::string& msg) : m_msg(msg) {}

        const std::string& GetMessage() const { return m_msg; }

    private:
        std::string m_msg;
    };

    class RefreshSceneGraph : public Enigma::Frameworks::ICommand
    {
    public:
        RefreshSceneGraph(std::vector<std::shared_ptr<Enigma::SceneGraph::Spatial>> flattened_spatial) : m_flattenedSpatial(
            std::move(flattened_spatial)) {}

        const std::vector<std::shared_ptr<Enigma::SceneGraph::Spatial>>& GetFlattenedSpatial() const { return m_flattenedSpatial; }

    protected:
        std::vector<std::shared_ptr<Enigma::SceneGraph::Spatial>> m_flattenedSpatial;
    };
}

#endif // LEVEL_EDITOR_COMMANDS_H
