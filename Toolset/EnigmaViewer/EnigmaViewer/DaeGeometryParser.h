/*********************************************************************
 * \file   DaeGeometryParser.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   September 2024
 *********************************************************************/
#ifndef DAE_GEOMETRY_PARSER_H
#define DAE_GEOMETRY_PARSER_H

#include "Geometries/GeometryDataStoreMapper.h"
#include "Geometries/GeometryId.h"
#include "MathLib/Matrix4.h"
#include <system_error>
#include <string>
#include <functional>
#include "pugixml.hpp"

namespace EnigmaViewer
{
    class DaeVertexWeightsParser;

    class DaeGeometryParser
    {
    public:
        DaeGeometryParser(const std::function<void(const std::string&)>& output_pipe);

        std::error_code parseGeometryInstanceNode(const pugi::xml_node& geometry_inst, const std::string& model_name);
        std::error_code parseSceneNodeForSkin(const pugi::xml_node& scene_node_xml, const std::string& model_name);

        [[nodiscard]] const Enigma::Geometries::GeometryId& getGeometryId() const { return m_geometryId; }
        [[nodiscard]] const std::string& getGeometryName() const { return m_geometryName; }
        [[nodiscard]] const std::string& getMeshMaterialId() const { return m_meshMaterialId; }
        [[nodiscard]] const std::string& getSkinMeshNodeName() const { return m_skinMeshNodeName; }
        [[nodiscard]] const std::vector<std::string>& getSkinBoneNames() const { return m_skinBoneNames; }
        [[nodiscard]] const Enigma::MathLib::Matrix4& getBindShapeMatrix() const { return m_bindShapeMatrix; }

    protected:
        std::error_code parseSingleGeometry(const pugi::xml_node& geometry_xml_node, const std::shared_ptr<DaeVertexWeightsParser>& weights_parser, const std::string& model_name);
        std::error_code parseSkinMesh(const pugi::xml_node& skin_node_xml, const std::string& model_name);

        void clearParsedData();

    protected:
        std::function<void(const std::string&)> m_outputPipe;
        Enigma::Geometries::GeometryId m_geometryId;
        std::string m_geometryName;
        std::string m_meshMaterialId;
        std::string m_skinMeshNodeName;
        std::vector<std::string> m_skinBoneNames;
        Enigma::MathLib::Matrix4 m_bindShapeMatrix;
    };
}

#endif // DAE_GEOMETRY_PARSER_H
