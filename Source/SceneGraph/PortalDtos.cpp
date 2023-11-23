#include "PortalDtos.h"
#include "PortalZoneNode.h"
#include "Portal.h"
#include "PortalManagementNode.h"

using namespace Enigma::SceneGraph;
using namespace Enigma::Engine;

static std::string TOKEN_ADJACENT_NODE_NAME = "AdjacentNodeName";
static std::string TOKEN_IS_PORTAL_OPEN = "IsPortalOpen";
static std::string TOKEN_OUTSIDE_NODE_NAME = "OutsideNodeName";

PortalZoneNodeDto::PortalZoneNodeDto() : LazyNodeDto()
{
    m_factoryDesc = FactoryDesc(PortalZoneNode::TYPE_RTTI.getName());
}

PortalZoneNodeDto::PortalZoneNodeDto(const LazyNodeDto& lazy_node_dto) : LazyNodeDto(lazy_node_dto)
{
    assert(Frameworks::Rtti::isExactlyOrDerivedFrom(m_factoryDesc.GetRttiName(), PortalZoneNode::TYPE_RTTI.getName()));
}

PortalZoneNodeDto PortalZoneNodeDto::fromGenericDto(const Engine::GenericDto& dto)
{
    PortalZoneNodeDto node_dto(LazyNodeDto::fromGenericDto(dto));
    return node_dto;
}

GenericDto PortalZoneNodeDto::toGenericDto() const
{
    GenericDto dto = LazyNodeDto::toGenericDto();
    return dto;
}

PortalDto::PortalDto() : SpatialDto(), m_isOpen(false)
{
    m_factoryDesc = FactoryDesc(Portal::TYPE_RTTI.getName());
}

PortalDto::PortalDto(const SpatialDto& spatial_dto) : SpatialDto(spatial_dto), m_isOpen(false)
{
    assert(Frameworks::Rtti::isExactlyOrDerivedFrom(m_factoryDesc.GetRttiName(), Portal::TYPE_RTTI.getName()));
}

PortalDto PortalDto::fromGenericDto(const GenericDto& dto)
{
    PortalDto portal_dto(SpatialDto::fromGenericDto(dto));
    if (auto v = dto.TryGetValue<std::string>(TOKEN_ADJACENT_NODE_NAME)) portal_dto.adjacentZoneNodeName() = v.value();
    if (auto v = dto.TryGetValue<bool>(TOKEN_IS_PORTAL_OPEN)) portal_dto.isOpen() = v.value();
    return portal_dto;
}

GenericDto PortalDto::toGenericDto()
{
    GenericDto dto = SpatialDto::toGenericDto();
    dto.AddOrUpdate(TOKEN_ADJACENT_NODE_NAME, m_adjacentZoneNodeName);
    dto.AddOrUpdate(TOKEN_IS_PORTAL_OPEN, m_isOpen);
    return dto;
}

PortalManagementNodeDto::PortalManagementNodeDto() : NodeDto()
{
    m_factoryDesc = FactoryDesc(PortalManagementNode::TYPE_RTTI.getName());
}

PortalManagementNodeDto::PortalManagementNodeDto(const NodeDto& node_dto) : NodeDto(node_dto)
{
    assert(Frameworks::Rtti::isExactlyOrDerivedFrom(m_factoryDesc.GetRttiName(), PortalManagementNode::TYPE_RTTI.getName()));
}

PortalManagementNodeDto PortalManagementNodeDto::fromGenericDto(const Engine::GenericDto& dto)
{
    PortalManagementNodeDto node_dto(NodeDto::fromGenericDto(dto));
    if (auto v = dto.TryGetValue<std::string>(TOKEN_OUTSIDE_NODE_NAME)) node_dto.outsideZoneNodeName() = v.value();
    return node_dto;
}

GenericDto PortalManagementNodeDto::toGenericDto()
{
    GenericDto dto = NodeDto::toGenericDto();
    dto.AddOrUpdate(TOKEN_OUTSIDE_NODE_NAME, m_outsideZoneNodeName);
    return dto;
}
