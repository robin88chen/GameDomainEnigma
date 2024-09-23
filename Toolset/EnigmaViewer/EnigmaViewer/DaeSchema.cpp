#include "DaeSchema.h"

using namespace EnigmaViewer;

#define TOKEN_ID "id"

std::unordered_map<std::string, std::string> DaeSchema::m_nodeIdNameMapping;

pugi::xml_node DaeSchema::findChildNodeWithId(const pugi::xml_node& node_root, const std::string& token_name, const std::string& id)
{
    return node_root.find_node([=](pugi::xml_node node) -> bool
        {
            if ((token_name == node.name()) && (node.attribute(TOKEN_ID))
                && (id == node.attribute(TOKEN_ID).as_string())) return true;
            return false;
        });
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
