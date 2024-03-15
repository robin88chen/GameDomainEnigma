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
    pawn.spatial().localTransform(Matrix4::IDENTITY).modelBound(geometry_bound).topLevel(true);
    pawn.primitive(primitive_id).asNative(id.name() + ".pawn@DataPath").constitute(
        PersistenceLevel::Store);
    return pawn.toPawnDto().toGenericDto();
}

PortalZoneNodeAssembler SceneGraphMaker::makeInsideZoneNode(const SpatialId& node_id, const SpatialId& portal_id, const std::vector<SpatialId>& children)
{
    BoundingVolume unit_bv(Box3::UNIT_BOX);
    PortalZoneNodeAssembler node_assembler(node_id);
    node_assembler.lazyNode().node().spatial().localTransform(Matrix4::IDENTITY).modelBound(unit_bv).topLevel(true);
    for (const auto& child : children)
    {
        node_assembler.lazyNode().node().child(child);
    }
    node_assembler.asDeferred(node_id.name() + ".node", "DataPath");
    node_assembler.portalParentId(portal_id);

    return node_assembler;
}

PortalZoneNodeAssembler SceneGraphMaker::makeOutsideRegion(const SpatialId& outside_id, const SpatialId& root_id, const SpatialId& portal_id, const SpatialId& inside_zone_id, const std::vector<SpatialId>& children)
{
    BoundingVolume unit_bv(Box3::UNIT_BOX);
    PortalZoneNodeAssembler node_assembler(outside_id);
    node_assembler.lazyNode().node().spatial().localTransform(Matrix4::IDENTITY).modelBound(unit_bv).topLevel(true);
    for (const auto& child : children)
    {
        node_assembler.lazyNode().node().child(child);
    }
    node_assembler.asDeferred(outside_id.name() + ".node", "DataPath");
    node_assembler.portalParentId(root_id);

    PortalAssembler portal_assembler(portal_id);
    portal_assembler.spatial().localTransform(Matrix4::IDENTITY).modelBound(PrimitiveMeshMaker::getDoorBound());
    portal_assembler.adjacentZoneNodeId(inside_zone_id).isOpen(true).asNative(portal_id.name() + ".portal@DataPath");
    node_assembler.lazyNode().node().child(portal_id, portal_assembler.toGenericDto());
    return node_assembler;
}

GenericDto SceneGraphMaker::makePortalManagementNode(const SpatialId& node_id, const PortalZoneNodeAssembler& outside_region, const PortalZoneNodeAssembler& inside_zone)
{
    BoundingVolume root_bv(Box3::UNIT_BOX);
    PortalManagementNodeAssembler root_assembler(node_id);
    root_assembler.node().spatial().localTransform(Matrix4::IDENTITY).modelBound(root_bv).topLevel(true);
    root_assembler.node().child(inside_zone.id(), inside_zone.toDeHydratedGenericDto()).child(outside_region.id(), outside_region.toDeHydratedGenericDto());
    root_assembler.asNative(node_id.name() + ".node@DataPath").outsideZoneNodeId(outside_region.id());

    return root_assembler.toGenericDto();
}
