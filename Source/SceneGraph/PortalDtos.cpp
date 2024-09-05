#include "PortalDtos.h"
#include "PortalZoneNode.h"
#include "Portal.h"
#include "PortalManagementNode.h"
#include "OutRegionNode.h"

using namespace Enigma::SceneGraph;
using namespace Enigma::Engine;

static std::string TOKEN_PORTAL_PARENT_ID = "PortalParentID";
static std::string TOKEN_ADJACENT_NODE_ID = "AdjacentNodeID";
static std::string TOKEN_IS_PORTAL_OPEN = "IsPortalOpen";
static std::string TOKEN_OUTSIDE_NODE_ID = "OutsideNodeID";
static std::string TOKEN_OWNER_MANAGEMENT_ID = "OwnerManagementID";

/*PortalZoneNodeDto::PortalZoneNodeDto() : LazyNodeDto()
{
    m_factoryDesc = FactoryDesc(PortalZoneNode::TYPE_RTTI.getName());
}

PortalZoneNodeDto::PortalZoneNodeDto(const Engine::GenericDto& dto) : LazyNodeDto(dto)
{
    assert(Frameworks::Rtti::isExactlyOrDerivedFrom(m_factoryDesc.GetRttiName(), PortalZoneNode::TYPE_RTTI.getName()));
    if (auto v = dto.tryGetValue<std::vector<std::string>>(TOKEN_PORTAL_PARENT_ID)) portalParentId(v.value());
}

PortalZoneNodeDto::PortalZoneNodeDto(const LazyNodeDto& lazy_node_dto) : LazyNodeDto(lazy_node_dto)
{
    assert(Frameworks::Rtti::isExactlyOrDerivedFrom(m_factoryDesc.GetRttiName(), PortalZoneNode::TYPE_RTTI.getName()));
}

GenericDto PortalZoneNodeDto::toGenericDto() const
{
    GenericDto dto = LazyNodeDto::toGenericDto();
    if (!m_portalParentId.empty()) dto.addOrUpdate(TOKEN_PORTAL_PARENT_ID, m_portalParentId.tokens());
    return dto;
}*/

/*OutRegionNodeDto::OutRegionNodeDto() : LazyNodeDto()
{
    m_factoryDesc = FactoryDesc(OutRegionNode::TYPE_RTTI.getName());
}

OutRegionNodeDto::OutRegionNodeDto(const Engine::GenericDto& dto) : LazyNodeDto(dto)
{
    assert(Frameworks::Rtti::isExactlyOrDerivedFrom(m_factoryDesc.GetRttiName(), OutRegionNode::TYPE_RTTI.getName()));
    if (auto v = dto.tryGetValue<std::vector<std::string>>(TOKEN_OWNER_MANAGEMENT_ID)) ownerManagementId(v.value());
}

OutRegionNodeDto::OutRegionNodeDto(const LazyNodeDto& lazy_node_dto) : LazyNodeDto(lazy_node_dto)
{
    assert(Frameworks::Rtti::isExactlyOrDerivedFrom(m_factoryDesc.GetRttiName(), OutRegionNode::TYPE_RTTI.getName()));
}

GenericDto OutRegionNodeDto::toGenericDto() const
{
    GenericDto dto = LazyNodeDto::toGenericDto();
    if (!m_ownerManagementId.empty()) dto.addOrUpdate(TOKEN_OWNER_MANAGEMENT_ID, m_ownerManagementId.tokens());
    return dto;
}*/

/*PortalDto::PortalDto() : SpatialDto(), m_isOpen(false)
{
    m_factoryDesc = FactoryDesc(Portal::TYPE_RTTI.getName());
}

PortalDto::PortalDto(const Engine::GenericDto& dto) : SpatialDto(dto)
{
    assert(Frameworks::Rtti::isExactlyOrDerivedFrom(m_factoryDesc.GetRttiName(), Portal::TYPE_RTTI.getName()));
    if (auto v = dto.tryGetValue<std::vector<std::string>>(TOKEN_ADJACENT_NODE_ID)) adjacentZoneNodeId(v.value());
    if (auto v = dto.tryGetValue<bool>(TOKEN_IS_PORTAL_OPEN)) isOpen(v.value());
}

PortalDto::PortalDto(const SpatialDto& spatial_dto) : SpatialDto(spatial_dto), m_isOpen(false)
{
    assert(Frameworks::Rtti::isExactlyOrDerivedFrom(m_factoryDesc.GetRttiName(), Portal::TYPE_RTTI.getName()));
}

GenericDto PortalDto::toGenericDto()
{
    GenericDto dto = SpatialDto::toGenericDto();
    if (!m_adjacentZoneNodeId.empty()) dto.addOrUpdate(TOKEN_ADJACENT_NODE_ID, m_adjacentZoneNodeId.tokens());
    dto.addOrUpdate(TOKEN_IS_PORTAL_OPEN, m_isOpen);
    return dto;
}*/

/*PortalManagementNodeDto::PortalManagementNodeDto() : NodeDto()
{
    m_factoryDesc = FactoryDesc(PortalManagementNode::TYPE_RTTI.getName());
}

PortalManagementNodeDto::PortalManagementNodeDto(const Engine::GenericDto& dto) : NodeDto(dto)
{
    assert(Frameworks::Rtti::isExactlyOrDerivedFrom(m_factoryDesc.GetRttiName(), PortalManagementNode::TYPE_RTTI.getName()));
    if (auto v = dto.tryGetValue<std::vector<std::string>>(TOKEN_OUTSIDE_NODE_ID)) outsideRegionId(v.value());
}

PortalManagementNodeDto::PortalManagementNodeDto(const NodeDto& node_dto) : NodeDto(node_dto)
{
    assert(Frameworks::Rtti::isExactlyOrDerivedFrom(m_factoryDesc.GetRttiName(), PortalManagementNode::TYPE_RTTI.getName()));
}

GenericDto PortalManagementNodeDto::toGenericDto()
{
    GenericDto dto = NodeDto::toGenericDto();
    if (m_outsideRegionId) dto.addOrUpdate(TOKEN_OUTSIDE_NODE_ID, m_outsideRegionId.value().tokens());
    return dto;
}*/
