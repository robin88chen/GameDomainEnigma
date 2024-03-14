#include "SceneGraphMaker.h"
#include "FileSystem/FileSystem.h"
#include "GameEngine/BoundingVolume.h"
#include "SceneGraph/SceneGraphDtos.h"
#include "SceneGraph/Node.h"
#include "PrimitiveMeshMaker.h"
#include "SceneGraph/Pawn.h"
#include "SceneGraph/PortalDtos.h"
#include "SceneGraph/PortalManagementNode.h"
#include "SceneGraph/PortalZoneNode.h"
#include "SceneGraph/Portal.h"
#include "SceneGraph/SceneGraphAssemblers.h"

using namespace Enigma::Engine;
using namespace Enigma::MathLib;
using namespace Enigma::SceneGraph;
using namespace Enigma::Renderer;
using namespace Enigma::FileSystem;

GenericDto SceneGraphMaker::makePawm(const SpatialId& id, const Enigma::Primitives::PrimitiveId& primitive_id, const
    BoundingVolume& geometry_bound)
{
    PawnAssembler pawn(id);
    pawn.primitive(primitive_id).localTransform(Matrix4::IDENTITY).modelBound(geometry_bound).topLevel(true).asNative(id.name() + ".pawn@DataPath").constitute(
        PersistenceLevel::Store);
    return pawn.toPawnDto().toGenericDto();
}

GenericDto SceneGraphMaker::makeInsideZoneNode(const SpatialId& node_id, const SpatialId& portal_id, const std::vector<SpatialId>& children)
{
    BoundingVolume unit_bv(Box3::UNIT_BOX);
    PortalZoneNodeDto node_dto;
    node_dto.id() = node_id;
    node_dto.portalParentId() = portal_id;
    node_dto.isTopLevel() = true;
    node_dto.factoryDesc() = FactoryDesc(PortalZoneNode::TYPE_RTTI.getName()).ClaimAsDeferred(node_id.name() + ".node", "DataPath");
    node_dto.localTransform() = Matrix4::IDENTITY;
    node_dto.worldTransform() = node_dto.localTransform();
    node_dto.modelBound() = unit_bv.serializeDto().toGenericDto();
    node_dto.worldBound() = BoundingVolume::CreateFromTransform(unit_bv, node_dto.worldTransform()).serializeDto().toGenericDto();
    node_dto.spatialFlag() = static_cast<unsigned>(Spatial::Spatial_Unlit);
    for (const auto& child : children)
    {
        node_dto.children().push_back(child);
    }
    //node_dto.parentId() = parent_id;
    return node_dto.toGenericDto();
}

GenericDto SceneGraphMaker::makeOutsideRegion(const SpatialId& outside_id, const SpatialId& root_id, const SpatialId& portal_id, const SpatialId& inside_zone_id, const std::vector<SpatialId>& children)
{
    BoundingVolume unit_bv(Box3::UNIT_BOX);
    PortalZoneNodeDto node_dto;
    node_dto.id() = outside_id;
    node_dto.portalParentId() = root_id;
    node_dto.isTopLevel() = true;
    node_dto.factoryDesc() = FactoryDesc(PortalZoneNode::TYPE_RTTI.getName()).ClaimAsDeferred(outside_id.name() + ".node", "DataPath");
    node_dto.localTransform() = Matrix4::IDENTITY;
    node_dto.worldTransform() = node_dto.localTransform();
    node_dto.modelBound() = unit_bv.serializeDto().toGenericDto();
    node_dto.worldBound() = BoundingVolume::CreateFromTransform(unit_bv, node_dto.worldTransform()).serializeDto().toGenericDto();
    node_dto.spatialFlag() = static_cast<unsigned>(Spatial::Spatial_Unlit);
    for (const auto& child : children)
    {
        node_dto.children().push_back(child);
    }
    //node_dto.parentId() = parent_id;

    // portal
    PortalDto portal_dto;
    portal_dto.id() = portal_id;
    portal_dto.factoryDesc() = FactoryDesc(Portal::TYPE_RTTI.getName()).ClaimAsNative(portal_id.name() + ".portal@DataPath");
    portal_dto.localTransform() = Matrix4::IDENTITY;
    portal_dto.worldTransform() = Matrix4::IDENTITY;
    portal_dto.modelBound() = PrimitiveMeshMaker::getDoorBound().serializeDto().toGenericDto();
    portal_dto.worldBound() = BoundingVolume::CreateFromTransform(PrimitiveMeshMaker::getDoorBound(), portal_dto.worldTransform()).serializeDto().toGenericDto();
    portal_dto.spatialFlag() = static_cast<unsigned>(Spatial::Spatial_Unlit);
    portal_dto.adjacentZoneNodeId() = inside_zone_id;
    portal_dto.isOpen() = true;

    node_dto.children().push_back({ portal_id, portal_dto.toGenericDto() });
    return node_dto.toGenericDto();
}

GenericDto SceneGraphMaker::makeSceneGraph(const SpatialId& root_id, const SpatialId& outside_region_id, const SpatialId
    & portal_id, const SpatialId& inside_zone_id)
{
    BoundingVolume root_bv(Box3::UNIT_BOX);
    PortalManagementNodeDto root_dto;
    root_dto.id() = root_id;
    //root_dto.name() = root_id.name();
    root_dto.factoryDesc() = FactoryDesc(PortalManagementNode::TYPE_RTTI.getName()).ClaimAsNative(root_id.name() + ".node@DataPath");
    root_dto.localTransform() = Matrix4::IDENTITY;
    root_dto.worldTransform() = Matrix4::IDENTITY;
    root_dto.modelBound() = root_bv.serializeDto().toGenericDto();
    root_dto.worldBound() = root_bv.serializeDto().toGenericDto();
    root_dto.isTopLevel() = true;
    root_dto.spatialFlag() = static_cast<unsigned>(Spatial::Spatial_Unlit);
    root_dto.outsideZoneNodeId() = outside_region_id;

    // inside zone node
    BoundingVolume unit_bv(Box3::UNIT_BOX);
    PortalZoneNodeDto zone_node_dto;
    zone_node_dto.id() = inside_zone_id;
    zone_node_dto.factoryDesc() = FactoryDesc(PortalZoneNode::TYPE_RTTI.getName()).ClaimAsInstanced(inside_zone_id.name() + ".node", "DataPath");
    zone_node_dto.localTransform() = Matrix4::IDENTITY;
    zone_node_dto.worldTransform() = root_dto.worldTransform() * zone_node_dto.localTransform();
    zone_node_dto.modelBound() = unit_bv.serializeDto().toGenericDto();
    zone_node_dto.worldBound() = BoundingVolume::CreateFromTransform(unit_bv, zone_node_dto.worldTransform()).serializeDto().toGenericDto();
    zone_node_dto.spatialFlag() = static_cast<unsigned>(Spatial::Spatial_Unlit);
    zone_node_dto.portalParentId() = portal_id;
    //zone_node_dto.parentId() = root_id;*/

    // outside region
    BoundingVolume outside_child_bv(Box3::UNIT_BOX);
    PortalZoneNodeDto outside_child1_dto;
    outside_child1_dto.id() = outside_region_id;
    outside_child1_dto.localTransform() = Matrix4::IDENTITY;
    outside_child1_dto.worldTransform() = root_dto.worldTransform() * outside_child1_dto.localTransform();
    outside_child1_dto.modelBound() = outside_child_bv.serializeDto().toGenericDto();
    outside_child1_dto.worldBound() = BoundingVolume::CreateFromTransform(outside_child_bv, outside_child1_dto.worldTransform()).serializeDto().toGenericDto();
    outside_child1_dto.spatialFlag() = static_cast<unsigned>(Spatial::Spatial_Unlit);
    //outside_child1_dto.parentId() = root_dto.id();
    outside_child1_dto.portalParentId() = root_id;
    root_dto.children().push_back({ inside_zone_id, zone_node_dto.toGenericDto() });
    root_dto.children().push_back({ outside_region_id, outside_child1_dto.toGenericDto() });
    return root_dto.toGenericDto();
}
