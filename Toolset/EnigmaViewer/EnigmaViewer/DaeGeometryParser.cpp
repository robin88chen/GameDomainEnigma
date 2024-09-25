#include "DaeGeometryParser.h"
#include "DaeMeshVerticesParser.h"
#include "DaeVertexWeightsParser.h"
#include "DaeParserErrors.h"
#include "DaeSchema.h"
#include <cassert>
#include <sstream>

using namespace EnigmaViewer;

#define TOKEN_URL "url"
#define TOKEN_GEOMETRY "geometry"
#define TOKEN_ID "id"
#define TOKEN_NAME "name"
#define TOKEN_MESH "mesh"
#define TOKEN_INSTANCE_CONTROLLER "instance_controller"
#define TOKEN_CONTROLLER "controller"
#define TOKEN_SKIN "skin"
#define TOKEN_BIND_SHAPE_MATRIX "bind_shape_matrix"
#define TOKEN_VERTEX_WEIGHTS "vertex_weights"
#define TOKEN_SOURCE "source"

DaeGeometryParser::DaeGeometryParser(const std::function<void(const std::string&)>& output_pipe, const std::shared_ptr<Enigma::Geometries::GeometryDataStoreMapper>& geometry_store)
    : m_outputPipe(output_pipe), m_geometryStore(geometry_store)
{
    assert(m_outputPipe);
}

std::error_code DaeGeometryParser::parseGeometryInstanceNode(const pugi::xml_node& geometry_inst, const std::string& model_name)
{
    std::string geometry_url = &(geometry_inst.attribute(TOKEN_URL).as_string()[1]);
    m_outputPipe("   Parse geometry instance " + geometry_url + "...");
    pugi::xml_node geometry_node = geometry_inst.root().find_node([=](pugi::xml_node node) -> bool
        {
            if ((strcmp(node.name(), TOKEN_GEOMETRY) == 0) && (node.attribute(TOKEN_ID))
                && (geometry_url == node.attribute(TOKEN_ID).as_string())) return true;
            return false;
        });
    if (!geometry_node)
    {
        m_outputPipe("can't find geometry " + geometry_url + "!!");
        return ParserError::noGeometryNode;
    }
    clearParsedData();
    return parseSingleGeometry(geometry_node, nullptr, model_name);
}

std::error_code DaeGeometryParser::parseSceneNodeForSkin(const pugi::xml_node& scene_node_xml, const std::string& model_name)
{
    std::string node_name = std::string(scene_node_xml.attribute(TOKEN_ID).as_string());
    m_outputPipe("   Parse skin mesh node " + node_name + "...");
    pugi::xml_node inst_controller = scene_node_xml.child(TOKEN_INSTANCE_CONTROLLER);
    if (!inst_controller)
    {
        m_outputPipe("   " + node_name + " is not a skin mesh node");
        return ParserError::notSkinGeometry;
    }

    m_skinMeshNodeName = scene_node_xml.attribute(TOKEN_NAME).as_string();

    std::string controller_id = &(inst_controller.attribute(TOKEN_URL).as_string()[1]);
    pugi::xml_node controller_node = DaeSchema::findChildNodeWithId(scene_node_xml.root(), TOKEN_CONTROLLER, controller_id);
    if (!controller_node)
    {
        m_outputPipe("can't find controller " + controller_id);
        return ParserError::noController;
    }
    pugi::xml_node skin_node = controller_node.child(TOKEN_SKIN);
    if (!skin_node)
    {
        m_outputPipe("no skin define!!");
        return ParserError::noControlledSkin;
    }
    clearParsedData();
    std::error_code er = parseSkinMesh(skin_node, model_name);
    return er;
}

std::error_code DaeGeometryParser::parseSingleGeometry(const pugi::xml_node& geometry_xml_node, const std::shared_ptr<DaeVertexWeightsParser>& weights_parser, const std::string& model_name)
{
    pugi::xml_attribute attr_name = geometry_xml_node.attribute(TOKEN_NAME);
    if (!attr_name)
    {
        m_outputPipe("geometry do not have name!!");
    }
    m_geometryName = model_name + "." + attr_name.as_string();
    m_geometryId = Enigma::Geometries::GeometryId("geometries/" + m_geometryName);
    m_outputPipe("  Parse Geometry " + std::string(attr_name.as_string()));
    DaeMeshVerticesParser vertices_parser(m_outputPipe);
    std::error_code err = vertices_parser.parseVertices(geometry_xml_node.child(TOKEN_MESH));
    if (err)
    {
        m_outputPipe("  Error parsing geometry vertices!!");
        return err;
    }
    m_meshMaterialId = vertices_parser.getMaterialId();
    err = vertices_parser.organizeVertices(weights_parser);
    if (err)
    {
        m_outputPipe("  Error organizing geometry vertices!!");
        return err;
    }
    if (!m_geometryStore.expired())
    {
        vertices_parser.persistSingleGeometry(m_geometryId, weights_parser, m_geometryStore.lock());
    }
    return ParserError::ok;
}

std::error_code DaeGeometryParser::parseSkinMesh(const pugi::xml_node& skin_node_xml, const std::string& model_name)
{
    m_bindShapeMatrix = Enigma::MathLib::Matrix4::IDENTITY;
    pugi::xml_node matrix_xml_node = skin_node_xml.child(TOKEN_BIND_SHAPE_MATRIX);
    if (matrix_xml_node)
    {
        std::stringstream ss(std::string(matrix_xml_node.text().as_string()));
        for (int i = 0; i < 16; i++)
        {
            ss >> ((float*)m_bindShapeMatrix)[i];
        }
    }

    pugi::xml_node vertex_weights_node_xml = skin_node_xml.child(TOKEN_VERTEX_WEIGHTS);
    if (!vertex_weights_node_xml)
    {
        m_outputPipe("no vertex weights define!!");
        return ParserError::noVertexWeights;
    }
    m_outputPipe("  Parse Vertex Weights for " + m_skinMeshNodeName);
    std::shared_ptr<DaeVertexWeightsParser> vertex_weights_parser = std::make_shared<DaeVertexWeightsParser>(m_outputPipe);
    std::error_code er = vertex_weights_parser->parseVertexWeights(skin_node_xml, vertex_weights_node_xml);
    if (er)
    {
        m_outputPipe("  Error parsing vertex weights!!");
        return er;
    }
    m_skinBoneNames = vertex_weights_parser->getSkinBoneNames();
    std::string geometry_url = &(skin_node_xml.attribute(TOKEN_SOURCE).as_string()[1]);
    pugi::xml_node geometry_node = DaeSchema::findChildNodeWithId(skin_node_xml.root(), TOKEN_GEOMETRY, geometry_url);
    if (!geometry_node)
    {
        m_outputPipe("can't find geometry " + geometry_url + "!!");
        return ParserError::noGeometryNode;
    }
    er = parseSingleGeometry(geometry_node, vertex_weights_parser, model_name);
    return er;
}

void DaeGeometryParser::clearParsedData()
{
}

