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

        [[nodiscard]] const std::vector<Engine::GenericDto>& quadTreeRoots() const { return m_quadTreeRoots; }
        [[nodiscard]] std::vector<Engine::GenericDto>& quadTreeRoots() { return m_quadTreeRoots; }
        [[nodiscard]] const std::vector<Engine::GenericDto>& nonLazyChildren() const { return m_nonLazyChildren; }
        [[nodiscard]] std::vector<Engine::GenericDto>& nonLazyChildren() { return m_nonLazyChildren; }

        static WorldMapDto fromGenericDto(const Engine::GenericDto& dto);
        Engine::GenericDto toGenericDto() const;

    protected:
        std::vector<Engine::GenericDto> m_quadTreeRoots;
        std::vector<Engine::GenericDto> m_nonLazyChildren;
    };
}

#endif // WORLD_MAP_DTO_H
