/*********************************************************************
 * \file   DaeSceneNodeTreeParser.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   September 2024
 *********************************************************************/
#ifndef DAE_SCENE_NODE_TREE_PARSER_H
#define DAE_SCENE_NODE_TREE_PARSER_H

#include "Renderables/MeshNodeAssemblers.h"
#include <functional>
#include <string>
#include <system_error>
#include "pugixml.hpp"

namespace EnigmaViewer
{
    class DaeSceneNodeTreeParser
    {
    public:
        DaeSceneNodeTreeParser(const std::function<void(const std::string&)>& output_pipe, const std::string& model_name);

        std::error_code parseSceneNodeTree(const pugi::xml_node& model_scene_root);

        [[nodiscard]] const std::shared_ptr<Enigma::Renderables::MeshNodeTreeAssembler>& getNodeTreeAssembler() const { return m_nodeTreeAssembler; }

    protected:
        std::error_code parseSceneNode(const pugi::xml_node& scene_node_xml, const std::optional<std::string>& parent_node_name);
        std::error_code parseSceneNodeForSkin(const pugi::xml_node& scene_node_xml, const std::optional<std::string>& parent_node_name);

    protected:
        std::function<void(const std::string&)> m_outputPipe;
        std::string m_modelName;
        std::shared_ptr<Enigma::Renderables::MeshNodeTreeAssembler> m_nodeTreeAssembler;
    };
}

#endif // DAE_SCENE_NODE_TREE_PARSER_H
