/*********************************************************************
 * \file   DaeSkinSceneNodeParser.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   September 2024
 *********************************************************************/
#ifndef DAE_SKIN_SCENE_NODE_PARSER_H
#define DAE_SKIN_SCENE_NODE_PARSER_H

#include "DaeSceneNodeParser.h"
#include "Renderables/MeshNodeAssemblers.h"
#include "Geometries/GeometryId.h"
#include "GameEngine/EffectMaterialId.h"
#include "GameEngine/TextureId.h"
#include <functional>
#include <string>
#include <system_error>
#include "pugixml.hpp"

namespace EnigmaViewer
{
    class DaeSkinSceneNodeParser : public DaeSceneNodeParser
    {
    public:
        DaeSkinSceneNodeParser(const std::function<void(const std::string&)>& output_pipe, const std::string& model_name);

        std::error_code parseSkinSceneNode(const pugi::xml_node& skin_scene_node);

        [[nodiscard]] const std::vector<std::string>& getSkinBoneNames() const { return m_skinBoneNames; }
    protected:
        void persistMesh(const Enigma::Primitives::PrimitiveId& mesh_id, const Enigma::Geometries::GeometryId& geo_id, const Enigma::Engine::EffectMaterialId& effect_id, const std::optional<Enigma::Engine::TextureId>& texture_id, const std::optional<std::string>& tex_semantic);

    protected:
        std::vector<std::string> m_skinBoneNames;
    };
}

#endif // DAE_SKIN_SCENE_NODE_PARSER_H
