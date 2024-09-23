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

    private:
        static std::unordered_map<std::string, std::string> m_nodeIdNameMapping;
    };
}

#endif // DAE_SCHEMA_H
