#include "PortalAssemblers.h"
#include "Portal.h"
#include "SceneGraphQueries.h"
#include "PortalZoneNode.h"
#include "PortalManagementNode.h"

using namespace Enigma::SceneGraph;

PortalAssembler::PortalAssembler(const SpatialId& id) : m_spatialAssembler(id)
{
    m_id = id;
    m_dto.id(id);
    m_dto.factoryDesc(Engine::FactoryDesc(Portal::TYPE_RTTI.getName()));
    m_spatialAssembler.factory(m_dto.factoryDesc());
}

SpatialAssembler& PortalAssembler::spatial()
{
    return m_spatialAssembler;
}

PortalAssembler& PortalAssembler::factory(const Engine::FactoryDesc& factory)
{
    m_dto.factoryDesc(factory);
    m_spatialAssembler.factory(factory);
    return *this;
}

PortalAssembler& PortalAssembler::adjacentZoneNodeId(const SpatialId& adjacent_zone_node_id)
{
    m_dto.adjacentZoneNodeId(adjacent_zone_node_id);
    return *this;
}

PortalAssembler& PortalAssembler::isOpen(bool is_open)
{
    m_dto.isOpen(is_open);
    return *this;
}

PortalAssembler& PortalAssembler::asNative(const std::string& file_at_path)
{
    auto fd = m_dto.factoryDesc();
    fd.ClaimAsNative(file_at_path);
    m_dto.factoryDesc(fd);
    m_spatialAssembler.factory(m_dto.factoryDesc());
    return *this;
}

PortalDto PortalAssembler::toPortalDto() const
{
    PortalDto portal_dto(m_spatialAssembler.toGenericDto());
    portal_dto.id(m_dto.id());
    portal_dto.factoryDesc(m_dto.factoryDesc());
    portal_dto.adjacentZoneNodeId(m_dto.adjacentZoneNodeId());
    portal_dto.isOpen(m_dto.isOpen());
    return portal_dto;
}

Enigma::Engine::GenericDto PortalAssembler::toGenericDto() const
{
    return toPortalDto().toGenericDto();
}

std::shared_ptr<Portal> PortalAssembler::constitute()
{
    return std::dynamic_pointer_cast<Portal>(std::make_shared<RequestSpatialConstitution>(m_id, toGenericDto())->dispatch());
}

PortalZoneNodeAssembler::PortalZoneNodeAssembler(const SpatialId& id) : m_lazyNodeAssembler(id)
{
    m_id = id;
    m_dto.id(id);
    m_dto.factoryDesc(Engine::FactoryDesc(PortalZoneNode::TYPE_RTTI.getName()));
    m_lazyNodeAssembler.factory(m_dto.factoryDesc());
}

LazyNodeAssembler& PortalZoneNodeAssembler::lazyNode()
{
    return m_lazyNodeAssembler;
}

PortalZoneNodeAssembler& PortalZoneNodeAssembler::factory(const Engine::FactoryDesc& factory)
{
    m_dto.factoryDesc(factory);
    m_lazyNodeAssembler.factory(factory);
    return *this;
}

PortalZoneNodeAssembler& PortalZoneNodeAssembler::portalParentId(const SpatialId& portal_parent_id)
{
    m_dto.portalParentId(portal_parent_id);
    return *this;
}

PortalZoneNodeAssembler& PortalZoneNodeAssembler::asDeferred(const std::string& filename, const std::string& path_id)
{
    auto fd = m_dto.factoryDesc();
    fd.ClaimAsDeferred(filename, path_id);
    m_dto.factoryDesc(fd);
    m_lazyNodeAssembler.factory(m_dto.factoryDesc());
    return *this;
}

PortalZoneNodeDto PortalZoneNodeAssembler::toHydratedDto() const
{
    PortalZoneNodeDto portal_zone_node_dto(m_lazyNodeAssembler.toHydratedDto());
    portal_zone_node_dto.id(m_dto.id());
    portal_zone_node_dto.factoryDesc(m_dto.factoryDesc());
    portal_zone_node_dto.portalParentId(m_dto.portalParentId());
    return portal_zone_node_dto;
}

Enigma::Engine::GenericDto PortalZoneNodeAssembler::toHydratedGenericDto() const
{
    return toHydratedDto().toGenericDto();
}

std::shared_ptr<PortalZoneNode> PortalZoneNodeAssembler::constituteHydrated()
{
    return std::dynamic_pointer_cast<PortalZoneNode>(std::make_shared<RequestSpatialConstitution>(m_id, toHydratedGenericDto())->dispatch());
}

PortalZoneNodeDto PortalZoneNodeAssembler::toDeHydratedDto() const
{
    PortalZoneNodeDto portal_zone_node_dto(m_lazyNodeAssembler.toDeHydratedDto());
    portal_zone_node_dto.id(m_dto.id());
    portal_zone_node_dto.factoryDesc(m_dto.factoryDesc());
    portal_zone_node_dto.portalParentId(m_dto.portalParentId());
    return portal_zone_node_dto;
}

Enigma::Engine::GenericDto PortalZoneNodeAssembler::toDeHydratedGenericDto() const
{
    return toDeHydratedDto().toGenericDto();
}

std::shared_ptr<PortalZoneNode> PortalZoneNodeAssembler::constituteDeHydrated()
{
    return std::dynamic_pointer_cast<PortalZoneNode>(std::make_shared<RequestSpatialConstitution>(m_id, toDeHydratedGenericDto())->dispatch());
}

OutRegionNodeAssembler::OutRegionNodeAssembler(const SpatialId& id) : m_lazyNodeAssembler(id)
{
    m_id = id;
    m_dto.id(id);
    m_dto.factoryDesc(Engine::FactoryDesc(OutRegionNode::TYPE_RTTI.getName()));
    m_lazyNodeAssembler.factory(m_dto.factoryDesc());
}

LazyNodeAssembler& OutRegionNodeAssembler::lazyNode()
{
    return m_lazyNodeAssembler;
}

OutRegionNodeAssembler& OutRegionNodeAssembler::factory(const Engine::FactoryDesc& factory)
{
    m_dto.factoryDesc(factory);
    m_lazyNodeAssembler.factory(factory);
    return *this;
}

OutRegionNodeAssembler& OutRegionNodeAssembler::ownerManagementId(const SpatialId& owner_id)
{
    m_dto.ownerManagementId(owner_id);
    return *this;
}

OutRegionNodeAssembler& OutRegionNodeAssembler::asDeferred(const std::string& filename, const std::string& path_id)
{
    auto fd = m_dto.factoryDesc();
    fd.ClaimAsDeferred(filename, path_id);
    m_dto.factoryDesc(fd);
    m_lazyNodeAssembler.factory(m_dto.factoryDesc());
    return *this;
}

OutRegionNodeDto OutRegionNodeAssembler::toHydratedDto() const
{
    OutRegionNodeDto region_node_dto(m_lazyNodeAssembler.toHydratedDto());
    region_node_dto.id(m_dto.id());
    region_node_dto.factoryDesc(m_dto.factoryDesc());
    region_node_dto.ownerManagementId(m_dto.ownerManagementId());
    return region_node_dto;
}

Enigma::Engine::GenericDto OutRegionNodeAssembler::toHydratedGenericDto() const
{
    return toHydratedDto().toGenericDto();
}

std::shared_ptr<OutRegionNode> OutRegionNodeAssembler::constituteHydrated()
{
    return std::dynamic_pointer_cast<OutRegionNode>(std::make_shared<RequestSpatialConstitution>(m_id, toHydratedGenericDto())->dispatch());
}

OutRegionNodeDto OutRegionNodeAssembler::toDeHydratedDto() const
{
    OutRegionNodeDto region_node_dto(m_lazyNodeAssembler.toDeHydratedDto());
    region_node_dto.id(m_dto.id());
    region_node_dto.factoryDesc(m_dto.factoryDesc());
    region_node_dto.ownerManagementId(m_dto.ownerManagementId());
    return region_node_dto;
}

Enigma::Engine::GenericDto OutRegionNodeAssembler::toDeHydratedGenericDto() const
{
    return toDeHydratedDto().toGenericDto();
}

std::shared_ptr<OutRegionNode> OutRegionNodeAssembler::constituteDeHydrated()
{
    return std::dynamic_pointer_cast<OutRegionNode>(std::make_shared<RequestSpatialConstitution>(m_id, toDeHydratedGenericDto())->dispatch());
}

PortalManagementNodeAssembler::PortalManagementNodeAssembler(const SpatialId& id) : m_nodeAssembler(id)
{
    m_id = id;
    m_dto.id(id);
    m_dto.factoryDesc(Engine::FactoryDesc(PortalManagementNode::TYPE_RTTI.getName()));
    m_nodeAssembler.factory(m_dto.factoryDesc());
}

NodeAssembler& PortalManagementNodeAssembler::node()
{
    return m_nodeAssembler;
}

PortalManagementNodeAssembler& PortalManagementNodeAssembler::factory(const Engine::FactoryDesc& factory)
{
    m_dto.factoryDesc(factory);
    m_nodeAssembler.factory(factory);
    return *this;
}

PortalManagementNodeAssembler& PortalManagementNodeAssembler::asNative(const std::string& file_at_path)
{
    auto fd = m_dto.factoryDesc();
    fd.ClaimAsNative(file_at_path);
    m_dto.factoryDesc(fd);
    m_nodeAssembler.factory(m_dto.factoryDesc());
    return *this;
}

PortalManagementNodeAssembler& PortalManagementNodeAssembler::outsideRegionId(const SpatialId& outside_region_id)
{
    m_dto.outsideRegionId(outside_region_id);
    return *this;
}

PortalManagementNodeDto PortalManagementNodeAssembler::toPortalManagementNodeDto() const
{
    PortalManagementNodeDto portal_management_node_dto(m_nodeAssembler.toNodeDto());
    portal_management_node_dto.id(m_dto.id());
    portal_management_node_dto.factoryDesc(m_dto.factoryDesc());
    if (m_dto.outsideRegionId()) portal_management_node_dto.outsideRegionId(m_dto.outsideRegionId().value());
    return portal_management_node_dto;
}

Enigma::Engine::GenericDto PortalManagementNodeAssembler::toGenericDto() const
{
    return toPortalManagementNodeDto().toGenericDto();
}
