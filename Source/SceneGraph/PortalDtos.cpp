#include "PortalDtos.h"

using namespace Enigma::SceneGraph;
using namespace Enigma::Engine;

static std::string TOKEN_ADJACENT_NODE_NAME = "AdjacentNodeName";
static std::string TOKEN_IS_PORTAL_OPEN = "IsPortalOpen";
static std::string TOKEN_OUTSIDE_NODE_NAME = "OutsideNodeName";

PortalZoneNodeDto::PortalZoneNodeDto(const LazyNodeDto& lazy_node_dto) : LazyNodeDto(lazy_node_dto)
{
}

PortalZoneNodeDto PortalZoneNodeDto::FromGenericDto(const Engine::GenericDto& dto)
{
    PortalZoneNodeDto node_dto(LazyNodeDto::FromGenericDto(dto));
    return node_dto;
}

GenericDto PortalZoneNodeDto::ToGenericDto() const
{
    GenericDto dto = LazyNodeDto::ToGenericDto();
    return dto;
}

PortalDto::PortalDto(const SpatialDto& spatial_dto) : SpatialDto(spatial_dto), m_isOpen(false)
{
}

PortalDto PortalDto::FromGenericDto(const GenericDto& dto)
{
    PortalDto portal_dto(SpatialDto::FromGenericDto(dto));
    if (auto v = dto.TryGetValue<std::string>(TOKEN_ADJACENT_NODE_NAME)) portal_dto.AdjacentZoneNodeName() = v.value();
    if (auto v = dto.TryGetValue<bool>(TOKEN_IS_PORTAL_OPEN)) portal_dto.IsOpen() = v.value();
    return portal_dto;
}

GenericDto PortalDto::ToGenericDto()
{
    GenericDto dto = SpatialDto::ToGenericDto();
    dto.AddOrUpdate(TOKEN_ADJACENT_NODE_NAME, m_adjacentZoneNodeName);
    dto.AddOrUpdate(TOKEN_IS_PORTAL_OPEN, m_isOpen);
    return dto;
}

PortalManagementNodeDto::PortalManagementNodeDto(const NodeDto& node_dto) : NodeDto(node_dto)
{
}

PortalManagementNodeDto PortalManagementNodeDto::FromGenericDto(const Engine::GenericDto& dto)
{
    PortalManagementNodeDto node_dto(NodeDto::FromGenericDto(dto));
    if (auto v = dto.TryGetValue<std::string>(TOKEN_OUTSIDE_NODE_NAME)) node_dto.OutsideZoneNodeName() = v.value();
    return node_dto;
}

GenericDto PortalManagementNodeDto::ToGenericDto()
{
    GenericDto dto = NodeDto::ToGenericDto();
    dto.AddOrUpdate(TOKEN_OUTSIDE_NODE_NAME, m_outsideZoneNodeName);
    return dto;
}
