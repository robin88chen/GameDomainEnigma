#include "WorldMapDto.h"
#include "WorldMap.h"

using namespace Enigma::WorldMap;
using namespace Enigma::Engine;

static std::string TOKEN_QUAD_TREE_ROOTS = "QuadTreeRoots";
static std::string TOKEN_NON_LAZY_CHILDREN = "NonLazyChildren";

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
    if (auto v = dto.TryGetValue<std::vector<Engine::GenericDto>>(TOKEN_QUAD_TREE_ROOTS)) world_map_dto.quadTreeRoots() = v.value();
    if (auto v = dto.TryGetValue<std::vector<Engine::GenericDto>>(TOKEN_NON_LAZY_CHILDREN)) world_map_dto.nonLazyChildren() = v.value();
    return world_map_dto;
}

Enigma::Engine::GenericDto WorldMapDto::toGenericDto() const
{
    GenericDto dto = PortalZoneNodeDto::ToGenericDto();
    dto.AddOrUpdate(TOKEN_QUAD_TREE_ROOTS, m_quadTreeRoots);
    if (!m_nonLazyChildren.empty()) dto.AddOrUpdate(TOKEN_NON_LAZY_CHILDREN, m_nonLazyChildren);
    return dto;
}
