/*********************************************************************
 * \file   DaeSchema.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   September 2024
 *********************************************************************/
#ifndef DAE_SCHEMA_H
#define DAE_SCHEMA_H

#include "pugixml.hpp"
#include <unordered_map>

namespace EnigmaViewer
{
    class DaeSchema
    {
    public:
        static pugi::xml_node findChildNodeWithId(const pugi::xml_node& node_root, const std::string& token_name, const std::string& id);

        static void clearNodeIdNameMapping();
        static void addNodeIdNameMapping(const std::string& node_id, const std::string& node_name);
        static const std::string& getNodeNameFromId(const std::string& node_id);

        static void clearNodeJointIdMapping();
        static void addNodeJointIdMapping(const std::string& joint_id, const std::string& mesh_name);
        static const std::string& getMeshNameFromJointId(const std::string& joint_id);

    private:
        static std::unordered_map<std::string, std::string> m_nodeIdNameMapping;
        static std::unordered_map<std::string, std::string> m_nodeJointIdMapping;
    };
}

#endif // DAE_SCHEMA_H
