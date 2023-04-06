/*********************************************************************
 * \file   DaeParser.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   April 2023
 *********************************************************************/
#ifndef _DAE_PARSER_H
#define _DAE_PARSER_H

#include "Renderer/ModelPrimitive.h"
#include "GameEngine/GeometryRepository.h"
#include "pugixml.hpp"
#include <string>
#include <optional>

namespace EnigmaViewer
{
    class DaeParser
    {
    public:
        DaeParser(const std::shared_ptr<Enigma::Engine::GeometryRepository>& geometry_repository);
        virtual ~DaeParser();

        void LoadDaeFile(const std::string& filename);

        Enigma::Renderer::ModelPrimitivePtr GetModelPrimitive() { return m_model; }

    private:
        void OutputLog(const std::string& msg);

        void ParseScene(const pugi::xml_node& collada_root);
        void ParseModelSceneNode(const pugi::xml_node& model_scene_node);
        void ParseSceneNode(const pugi::xml_node& scene_node_xml, std::optional<unsigned> parent_node_array_index);

        void ParseGeometryInstanceNode(Enigma::Renderer::MeshNode& mesh_node, const pugi::xml_node& geometry_inst);
        void ParseSingleGeometry(Enigma::Renderer::MeshNode& mesh_node, const pugi::xml_node& geometry_node, bool is_skin);

        void ParseGeometryMesh(const pugi::xml_node& mesh_node, std::string& material_id, int& pos_offset, int& nor_offset, int tex_offset[4]);
        void ParsePositionSource(const pugi::xml_node& position_source);
        void ParseNormalSource(const pugi::xml_node& normal_source);
        void ParseTexcoordSource(const pugi::xml_node& texcoord_source, int texcoord_set);
        void ParseIndexArray(const pugi::xml_node& index_ary_node, int triangle_count);

        pugi::xml_node FindNodeWithId(const pugi::xml_node& node_root, const std::string& token_name,
            const std::string& id);
        void ClearParsedDatas();

    private:
        std::weak_ptr<Enigma::Engine::GeometryRepository> m_geometryRepository;

        std::string m_filename;
        Enigma::Renderer::ModelPrimitivePtr m_model;

        std::vector<Enigma::MathLib::Vector3> m_positions;
        std::vector<Enigma::MathLib::Vector3> m_normals;
        std::vector<Enigma::MathLib::Vector2> m_texcoords[4];
        std::vector<int> m_primitiveIndices;

        std::unordered_map<std::string, std::string> m_nodeIdNameMapping;
        std::unordered_map<std::string, std::string> m_nodeJointIdMapping;
    };
}
#endif // DAEPARSER_H
