#include "DaeSceneParser.h"
#include "DaeParserErrors.h"
#include "DaeSceneNodeTreeParser.h"
#include "FileSystem/Filename.h"
#include "Renderables/ModelPrimitive.h"
#include "Primitives/PrimitiveId.h"
#include "Renderables/ModelPrimitiveAssembler.h"
#include "Renderables/MeshNodeAssemblers.h"
#include <cassert>

using namespace EnigmaViewer;

#define TOKEN_SCENE "scene"
#define TOKEN_INSTANCE_SCENE "instance_visual_scene"
#define TOKEN_URL "url"
#define TOKEN_SCENE_NODE "visual_scene"
#define TOKEN_ID "id"
#define TOKEN_NAME "name"
#define TOKEN_NODE "node"

DaeSceneParser::DaeSceneParser(const std::function<void(const std::string&)>& output_pipe)
    : m_outputPipe(output_pipe)
{
    assert(m_outputPipe);
}

std::error_code DaeSceneParser::parseScene(const pugi::xml_node& collada_root, const std::string& filename)
{
    if (!collada_root) return ParserError::emptyRoot;
    m_outputPipe("Start parse scene...");
    pugi::xml_node scene_node = collada_root.child(TOKEN_SCENE);
    if (!scene_node)
    {
        m_outputPipe(filename + " do not have any scene!!");
        return ParserError::noSceneNode;
    }
    pugi::xml_node inst_scene = scene_node.child(TOKEN_INSTANCE_SCENE);
    if (!inst_scene)
    {
        m_outputPipe(filename + " do not have any instanced scene!!");
        return ParserError::noInstanceScene;
    }
    if (!inst_scene.attribute(TOKEN_URL))
    {
        m_outputPipe("instanced scene url error!!");
        return ParserError::instanceSceneUrlError;
    }
    std::string scene_id = &(inst_scene.attribute(TOKEN_URL).as_string()[1]);
    pugi::xml_node model_scene_node = collada_root.find_node([=](pugi::xml_node node) -> bool
        {
            if ((strcmp(node.name(), TOKEN_SCENE_NODE) == 0) && (node.attribute(TOKEN_ID))
                && (scene_id == node.attribute(TOKEN_ID).as_string())) return true;
            return false;
        });
    if (!model_scene_node)
    {
        m_outputPipe("can't find visual scene node " + scene_id + "!!");
        return ParserError::noModelSceneNode;
    }
    std::error_code er = parseModelSceneNode(model_scene_node, filename);
    return er;
}

std::error_code DaeSceneParser::parseModelSceneNode(const pugi::xml_node& model_scene_node, const std::string& filename)
{
    m_outputPipe(" Parse model scene node "
        + std::string(model_scene_node.attribute(TOKEN_ID).as_string()) + "...");
    std::string model_name = model_scene_node.attribute(TOKEN_NAME).as_string();
    if (model_name == "Scene")
    {
        model_name = Enigma::FileSystem::Filename(filename).getBaseFileName();
    }
    m_modelName = model_name;
    m_modelId = Enigma::Primitives::PrimitiveId("renderables/" + model_name, Enigma::Renderables::ModelPrimitive::TYPE_RTTI);
    m_modelAssembler = std::make_shared<Enigma::Renderables::ModelPrimitiveAssembler>(m_modelId);
    DaeSceneNodeTreeParser scene_node_tree_parser(m_outputPipe, m_modelName);
    std::error_code error = scene_node_tree_parser.parseSceneNodeTree(model_scene_node);
    if (error)
    {
        m_outputPipe("Error parsing scene node tree!!");
        return error;
    }
    m_modelAssembler->meshNodeTree(scene_node_tree_parser.getNodeTreeAssembler());
    return ParserError::ok;
}
