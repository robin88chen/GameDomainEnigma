/*********************************************************************
 * \file   DaeSceneNodeParser.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   September 2024
 *********************************************************************/
#ifndef DAE_SCENE_NODE_PARSER_H
#define DAE_SCENE_NODE_PARSER_H

#include "Renderables/MeshNodeAssemblers.h"
#include "DaeParserConfiguration.h"
#include "Geometries/GeometryId.h"
#include "GameEngine/EffectMaterialId.h"
#include "GameEngine/TextureId.h"
#include <functional>
#include <string>
#include <system_error>
#include "pugixml.hpp"

namespace EnigmaViewer
{
    class DaeSceneNodeParser
    {
    public:
        DaeSceneNodeParser(const std::function<void(const std::string&)>& output_pipe, const std::string& model_name);

        std::error_code parseSceneNode(const pugi::xml_node& scene_node_xml, const std::string& mesh_node_name);
        std::error_code parseSkinSceneNode(const pugi::xml_node& scene_node_xml, const std::string& mesh_node_name);

        [[nodiscard]] const std::shared_ptr<Enigma::Renderables::MeshNodeAssembler>& getNodeAssembler() const { return m_nodeAssembler; }

    protected:
        std::error_code parseGeometryInstanceNode(const pugi::xml_node& geometry_inst);

        std::string findMaterialTexImageFilename(const pugi::xml_node& collada_root, const std::string& material_id);

        void persistMesh(const Enigma::Primitives::PrimitiveId& mesh_id, const Enigma::Geometries::GeometryId& geo_id, const Enigma::Engine::EffectMaterialId& effect_id, const std::optional<Enigma::Engine::TextureId>& texture_id, const std::optional<std::string>& tex_semantic);

    protected:
        DaeParserConfiguration m_config;
        std::function<void(const std::string&)> m_outputPipe;
        std::string m_modelName;
        std::shared_ptr<Enigma::Renderables::MeshNodeAssembler> m_nodeAssembler;
    };
}

#endif // DAE_SCENE_NODE_PARSER_H
