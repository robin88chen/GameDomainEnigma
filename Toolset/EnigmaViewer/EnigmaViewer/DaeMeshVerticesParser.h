/*********************************************************************
 * \file   DaeMeshVerticesParser.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   September 2024
 *********************************************************************/
#ifndef DAE_MESH_VERTICES_PARSER_H
#define DAE_MESH_VERTICES_PARSER_H

#include "MathLib/Vector3.h"
#include "MathLib/Vector2.h"
#include "Geometries/GeometryId.h"
#include "Geometries/GeometryDataStoreMapper.h"
#include "pugixml.hpp"
#include <system_error>
#include <string>
#include <functional>
#include <tuple>
#include <memory>

namespace EnigmaViewer
{
    class DaeVertexWeightsParser;

    class DaeMeshVerticesParser
    {
    public:
        DaeMeshVerticesParser(const std::function<void(const std::string&)>& output_pipe);

        std::error_code parseVertices(const pugi::xml_node& vertices_xml_node);
        std::error_code organizeVertices(const std::shared_ptr<DaeVertexWeightsParser>& weights_parser);
        void persistSingleGeometry(const Enigma::Geometries::GeometryId& geo_id, const std::shared_ptr<DaeVertexWeightsParser>& weights_parser, const std::shared_ptr<Enigma::Geometries::GeometryDataStoreMapper>& geometry_store);

        [[nodiscard]] const std::string& getMaterialId() const { return m_meshMaterialId; }
    protected:
        std::error_code parsePositionSource(const pugi::xml_node& position_source);
        std::error_code parseNormalSource(const pugi::xml_node& normal_source);
        std::error_code parseTexcoordSource(const pugi::xml_node& texcoord_source, int texcoord_set);
        std::error_code parseIndexArray(const pugi::xml_node& index_ary_node, int triangle_count);

        std::error_code splitVertexPositions(int pos_offset, int tex_set, int tex_offset, bool is_split_vertex, const std::shared_ptr<DaeVertexWeightsParser>& weights_parser);
        std::error_code collapseVertexNormals(int pos_offset, int nor_offset, bool is_split_vertex);
        std::error_code collapseIndexArray(int pos_offset, bool is_split_vertex);

    protected:
        struct GeometryValueOffsets
        {
            int pos_offset;
            int nor_offset;
            int tex_offset[4];
        };
        GeometryValueOffsets m_geometryValueOffsets;
        std::string m_meshMaterialId;

        std::function<void(const std::string&)> m_outputPipe;
        std::vector<Enigma::MathLib::Vector3> m_positions;
        std::vector<Enigma::MathLib::Vector3> m_normals;
        std::vector<Enigma::MathLib::Vector2> m_texcoords[4];
        std::vector<unsigned> m_primitiveIndices;

        std::vector<Enigma::MathLib::Vector3> m_splitedPositions;
        std::vector<Enigma::MathLib::Vector3> m_collapsedNormals;
        std::vector<Enigma::MathLib::Vector2> m_splitedTexCoord[4];
        std::vector<unsigned> m_collapsedIndices;
    };
}

#endif // DAE_MESH_VERTICES_PARSER_H
