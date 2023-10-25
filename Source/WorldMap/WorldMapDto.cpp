#include "WorldMapDto.h"
#include "WorldMap.h"

using namespace Enigma::WorldMap;
using namespace Enigma::Engine;

//static std::string TOKEN_WORLD_ZONE_NAME = "WorldZoneName";

WorldMapDto::WorldMapDto() : PortalZoneNodeDto()
{
    m_factoryDesc = FactoryDesc(WorldMap::TYPE_RTTI.getName());
}

WorldMapDto::WorldMapDto(const SceneGraph::PortalZoneNodeDto& portal_zone_node_dto)
    : PortalZoneNodeDto(portal_zone_node_dto)
{
    assert(Frameworks::Rtti::isExactlyOrDerivedFrom(m_factoryDesc.GetRttiName(), WorldMap::TYPE_RTTI.getName()));
    //m_factoryDesc = FactoryDesc(WorldMap::TYPE_RTTI.getName());
}

WorldMapDto WorldMapDto::fromGenericDto(const Engine::GenericDto& dto)
{
    WorldMapDto world_map_dto(PortalZoneNodeDto::FromGenericDto(dto));
    //world_map_dto.Name() = dto.getName();
    //if (auto v = dto.TryGetValue<std::string>(TOKEN_WORLD_ZONE_NAME)) world_map_dto.WorldZoneName() = v.value();
    return world_map_dto;
}

Enigma::Engine::GenericDto WorldMapDto::toGenericDto() const
{
    GenericDto dto = PortalZoneNodeDto::ToGenericDto();
    //dto.AddName(m_name);
    //dto.AddRtti(FactoryDesc(WorldMap::TYPE_RTTI.getName()));
    //dto.AddOrUpdate(TOKEN_WORLD_ZONE_NAME, m_worldZoneName);
    return dto;
}
