#include "DaeSchema.h"

using namespace EnigmaViewer;

#define TOKEN_ID "id"

std::unordered_map<std::string, std::string> DaeSchema::m_nodeIdNameMapping;
std::unordered_map<std::string, std::string> DaeSchema::m_nodeJointIdMapping;
std::unordered_map<std::string, Enigma::Primitives::PrimitiveId> DaeSchema::m_meshIdInMeshNode;
std::unordered_map<std::string, std::vector<std::string>> DaeSchema::m_skinBoneNames;

pugi::xml_node DaeSchema::findChildNodeWithId(const pugi::xml_node& node_root, const std::string& token_name, const std::string& id)
{
    return node_root.find_node([=](pugi::xml_node node) -> bool
        {
            if ((token_name == node.name()) && (node.attribute(TOKEN_ID))
                && (id == node.attribute(TOKEN_ID).as_string())) return true;
            return false;
        });
}

void DaeSchema::clear()
{
    clearNodeIdNameMapping();
    clearNodeJointIdMapping();
    clearMeshIdInMeshNode();
    clearSkinBoneNames();
}

void DaeSchema::clearNodeIdNameMapping()
{
    m_nodeIdNameMapping.clear();
}

void DaeSchema::addNodeIdNameMapping(const std::string& node_id, const std::string& node_name)
{
    m_nodeIdNameMapping[node_id] = node_name;
}

const std::string& DaeSchema::getNodeNameFromId(const std::string& node_id)
{
    return m_nodeIdNameMapping[node_id];
}

void DaeSchema::clearNodeJointIdMapping()
{
    m_nodeJointIdMapping.clear();
}

void DaeSchema::addNodeJointIdMapping(const std::string& node_joint_id, const std::string& mesh_name)
{
    m_nodeJointIdMapping[node_joint_id] = mesh_name;
}

const std::string& DaeSchema::getMeshNameFromJointId(const std::string& joint_id)
{
    return m_nodeJointIdMapping[joint_id];
}

void DaeSchema::clearMeshIdInMeshNode()
{
    m_meshIdInMeshNode.clear();
}

void DaeSchema::addMeshIdInMeshNodeName(const std::string& mesh_name, Enigma::Primitives::PrimitiveId mesh_node_id)
{
    m_meshIdInMeshNode[mesh_name] = mesh_node_id;
}

std::optional<Enigma::Primitives::PrimitiveId> DaeSchema::getMeshIdFromMeshNodeName(const std::string& mesh_node_name)
{
    if (m_meshIdInMeshNode.find(mesh_node_name) == m_meshIdInMeshNode.end())
    {
        return std::nullopt;
    }
    return m_meshIdInMeshNode[mesh_node_name];
}

void DaeSchema::clearSkinBoneNames()
{
    m_skinBoneNames.clear();
}

void DaeSchema::addSkinBoneNames(const std::string& skin_name, const std::vector<std::string>& bone_names)
{
    m_skinBoneNames[skin_name] = bone_names;
}

const std::unordered_map<std::string, std::vector<std::string>>& DaeSchema::getSkinBoneNames()
{
    return m_skinBoneNames;
}
