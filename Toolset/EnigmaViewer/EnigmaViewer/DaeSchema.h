/*********************************************************************
 * \file   DaeSchema.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   September 2024
 *********************************************************************/
#ifndef DAE_SCHEMA_H
#define DAE_SCHEMA_H

#include "Primitives/PrimitiveId.h"
#include "pugixml.hpp"
#include <unordered_map>
#include <optional>

namespace EnigmaViewer
{
    class DaeSchema
    {
    public:
        static pugi::xml_node findChildNodeWithId(const pugi::xml_node& node_root, const std::string& token_name, const std::string& id);

        static void clear();
        static void clearNodeIdNameMapping();
        static void addNodeIdNameMapping(const std::string& node_id, const std::string& node_name);
        static const std::string& getNodeNameFromId(const std::string& node_id);

        static void clearNodeJointIdMapping();
        static void addNodeJointIdMapping(const std::string& joint_id, const std::string& mesh_name);
        static const std::string& getMeshNameFromJointId(const std::string& joint_id);

        static void clearMeshIdInMeshNode();
        static void addMeshIdInMeshNodeName(const std::string& mesh_name, Enigma::Primitives::PrimitiveId mesh_node_id);
        static std::optional<Enigma::Primitives::PrimitiveId> getMeshIdFromMeshNodeName(const std::string& mesh_node_name);

        static void clearSkinBoneNames();
        static void addSkinBoneNames(const std::string& skin_name, const std::vector<std::string>& bone_names);
        static const std::unordered_map<std::string, std::vector<std::string>>& getSkinBoneNames();

    private:
        static std::unordered_map<std::string, std::string> m_nodeIdNameMapping;
        static std::unordered_map<std::string, std::string> m_nodeJointIdMapping;
        static std::unordered_map<std::string, Enigma::Primitives::PrimitiveId> m_meshIdInMeshNode;
        static std::unordered_map<std::string, std::vector<std::string>> m_skinBoneNames;
    };
}

#endif // DAE_SCHEMA_H
