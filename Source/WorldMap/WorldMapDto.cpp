#include "WorldMapDto.h"
#include "WorldMap.h"

using namespace Enigma::WorldMap;
using namespace Enigma::Engine;

//static std::string TOKEN_WORLD_ZONE_NAME = "WorldZoneName";

WorldMapDto WorldMapDto::FromGenericDto(const Engine::GenericDto& dto)
{
    WorldMapDto world_map_dto;
    world_map_dto.PortalZoneNodeDto::FromGenericDto(dto);
    //world_map_dto.Name() = dto.GetName();
    //if (auto v = dto.TryGetValue<std::string>(TOKEN_WORLD_ZONE_NAME)) world_map_dto.WorldZoneName() = v.value();
    return world_map_dto;
}

Enigma::Engine::GenericDto WorldMapDto::ToGenericDto() const
{
    GenericDto dto = PortalZoneNodeDto::ToGenericDto();
    //dto.AddName(m_name);
    dto.AddRtti(FactoryDesc(WorldMap::TYPE_RTTI.GetName()));
    //dto.AddOrUpdate(TOKEN_WORLD_ZONE_NAME, m_worldZoneName);
    return dto;
}
