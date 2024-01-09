#include "WorldMapDto.h"
#include "WorldMap.h"

using namespace Enigma::WorldMap;
using namespace Enigma::Engine;

static std::string TOKEN_NAME = "Name";
static std::string TOKEN_QUAD_TREE_ROOTS = "QuadTreeRoots";
static std::string TOKEN_PORTAL_ROOT = "PortalRoot";

WorldMapDto::WorldMapDto() : m_factory_desc(WorldMap::TYPE_RTTI.getName())
{
}

WorldMapDto WorldMapDto::fromGenericDto(const Engine::GenericDto& dto)
{
    WorldMapDto world_map_dto;
    world_map_dto.factoryDesc() = dto.getRtti();
    if (auto v = dto.TryGetValue<std::string>(TOKEN_NAME)) world_map_dto.name() = v.value();
    if (auto v = dto.TryGetValue<Engine::GenericDtoCollection>(TOKEN_QUAD_TREE_ROOTS)) world_map_dto.quadTreeRoots() = v.value();
    if (auto v = dto.TryGetValue<Engine::GenericDto>(TOKEN_PORTAL_ROOT)) world_map_dto.portalRoot() = v.value();
    return world_map_dto;
}

Enigma::Engine::GenericDto WorldMapDto::toGenericDto() const
{
    GenericDto dto;
    dto.AsTopLevel(true);
    dto.addRtti(m_factory_desc);
    dto.AddOrUpdate(TOKEN_NAME, m_name);
    dto.AddOrUpdate(TOKEN_QUAD_TREE_ROOTS, m_quadTreeRoots);
    dto.AddOrUpdate(TOKEN_PORTAL_ROOT, m_portalRoot);
    return dto;
}
