#include "DaeSceneNodeTreeParser.h"
#include "DaeSceneNodeParser.h"
#include "DaeSkinSceneNodeParser.h"
#include "DaeParserErrors.h"
#include "DaeSchema.h"
#include <cassert>

using namespace EnigmaViewer;

#define TOKEN_NODE "node"
#define TOKEN_ID "id"
#define TOKEN_INSTANCE_CONTROLLER "instance_controller"
#define TOKEN_NAME "name"

DaeSceneNodeTreeParser::DaeSceneNodeTreeParser(const std::function<void(const std::string&)>& output_pipe, const std::string& model_name)
    : m_outputPipe(output_pipe), m_modelName(model_name)
{
    assert(m_outputPipe);
}

std::error_code DaeSceneNodeTreeParser::parseSceneNodeTree(const pugi::xml_node& model_scene_root)
{
    m_nodeTreeAssembler = std::make_shared<Enigma::Renderables::MeshNodeTreeAssembler>();
    pugi::xml_node scene_node_xml = model_scene_root.child(TOKEN_NODE);
    while (scene_node_xml)
    {
        std::error_code err = parseSceneNode(scene_node_xml, std::nullopt);
        if (err) return err;
        scene_node_xml = scene_node_xml.next_sibling(TOKEN_NODE);
    }
    return ParserError::ok;
}

std::error_code DaeSceneNodeTreeParser::parseSceneNode(const pugi::xml_node& scene_node_xml, const std::optional<std::string>& parent_node_name)
{
    assert(m_nodeTreeAssembler);
    std::string node_xml_id = std::string(scene_node_xml.attribute(TOKEN_ID).as_string());
    m_outputPipe("   Parse scene node " + node_xml_id + "...");
    pugi::xml_node inst_controller = scene_node_xml.child(TOKEN_INSTANCE_CONTROLLER);
    if (inst_controller)
    {
        m_outputPipe("   " + node_xml_id + " is a skin mesh node");
        auto er = parseSceneNodeForSkin(scene_node_xml, parent_node_name);
        return er;  // this is a skin mesh node
    }

    // ADR: 這些跟 Tree 有關，所以要放在 TreeParser 裡面
    std::string mesh_node_name = scene_node_xml.attribute(TOKEN_NAME).as_string();
    DaeSchema::addNodeIdNameMapping(node_xml_id, mesh_node_name);
    std::string node_joint_name = scene_node_xml.attribute("sid").as_string();
    DaeSchema::addNodeJointIdMapping(node_joint_name, mesh_node_name);
    m_outputPipe("   scene node " + node_xml_id + " with mesh name " + mesh_node_name + ", joint name " + node_joint_name);

    DaeSceneNodeParser scene_node_parser(m_outputPipe, m_modelName);
    std::error_code err = scene_node_parser.parseSceneNode(scene_node_xml, mesh_node_name);
    if (err) return err;

    if (parent_node_name)
    {
        m_nodeTreeAssembler->addNodeWithParentName(mesh_node_name, scene_node_parser.getNodeAssembler(), parent_node_name.value());
    }
    else
    {
        m_nodeTreeAssembler->addNode(mesh_node_name, scene_node_parser.getNodeAssembler());
    }
    pugi::xml_node child_node = scene_node_xml.child(TOKEN_NODE);
    while (child_node)
    {
        auto er = parseSceneNode(child_node, mesh_node_name);
        if (er) return er;
        child_node = child_node.next_sibling(TOKEN_NODE);
    }
    return ParserError::ok;
}

std::error_code DaeSceneNodeTreeParser::parseSceneNodeForSkin(const pugi::xml_node& scene_node_xml, const std::optional<std::string>& parent_node_name)
{
    DaeSkinSceneNodeParser skin_scene_node_parser(m_outputPipe, m_modelName);
    std::error_code er = skin_scene_node_parser.parseSkinSceneNode(scene_node_xml);
    if (er)
    {
        m_outputPipe("parse skin mesh node error!!");
        return er;
    }
    std::string mesh_node_name = skin_scene_node_parser.getNodeAssembler()->name();
    if (parent_node_name)
    {
        m_nodeTreeAssembler->addNodeWithParentName(mesh_node_name, skin_scene_node_parser.getNodeAssembler(), parent_node_name.value());
    }
    else
    {
        m_nodeTreeAssembler->addNode(mesh_node_name, skin_scene_node_parser.getNodeAssembler());
    }
    return er;
}
