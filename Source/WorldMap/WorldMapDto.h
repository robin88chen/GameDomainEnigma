/*********************************************************************
 * \file   WorldMapDto.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   July 2023
 *********************************************************************/
#ifndef WORLD_MAP_DTO_H
#define WORLD_MAP_DTO_H
#include "SceneGraph/PortalDtos.h"

namespace Enigma::WorldMap
{
    class WorldMapDto : public SceneGraph::PortalZoneNodeDto
    {
    public:
        WorldMapDto();
        WorldMapDto(const SceneGraph::PortalZoneNodeDto& portal_zone_node_dto);

        //const std::string& Name() const { return m_name; }
        //std::string& Name() { return m_name; }

        static WorldMapDto FromGenericDto(const Engine::GenericDto& dto);
        Engine::GenericDto ToGenericDto() const;
        //[[nodiscard]] const std::string& WorldZoneName() const { return m_worldZoneName; }
        //std::string& WorldZoneName() { return m_worldZoneName; }

    protected:
        //std::string m_name;
        //std::string m_worldZoneName;
    };
}

#endif // WORLD_MAP_DTO_H
