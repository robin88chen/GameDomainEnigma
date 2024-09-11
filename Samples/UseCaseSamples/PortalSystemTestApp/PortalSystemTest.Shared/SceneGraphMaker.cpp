#include "SceneGraphMaker.h"
#include "SceneGraph/SceneGraphQueries.h"
#include "FileSystem/FileSystem.h"
#include "GameEngine/BoundingVolume.h"
#include "SceneGraph/PawnAssembler.h"
#include "PrimitiveMeshMaker.h"
#include "SceneGraph/Pawn.h"
#include "SceneGraph/SceneGraphCommands.h"
#include "SceneGraph/PortalAssembler.h"
#include "SceneGraph/PortalZoneNode.h"
#include "SceneGraph/PortalZoneNodeAssembler.h"
#include "SceneGraph/PortalManagementNodeAssembler.h"

using namespace Enigma::Engine;
using namespace Enigma::MathLib;
using namespace Enigma::SceneGraph;
using namespace Enigma::FileSystem;

GenericDto SceneGraphMaker::makePawn(const SpatialId& id, const Enigma::Primitives::PrimitiveId& primitive_id, const
    BoundingVolume& geometry_bound)
{
    std::shared_ptr<PawnAssembler> pawn_assembler = std::make_shared<PawnAssembler>(id);
    pawn_assembler->localTransform(Matrix4::IDENTITY);
    pawn_assembler->modelBound(geometry_bound);
    pawn_assembler->topLevel(true);
    pawn_assembler->primitiveId(primitive_id);
    pawn_assembler->persist(id.name() + ".pawn", "DataPath");
    return pawn_assembler->assemble();
}

std::shared_ptr<HydratedPortalZoneNodeAssembler> SceneGraphMaker::makeHydratedInsideZoneNode(const SpatialId& node_id, const SpatialId& portal_id, const std::vector<SpatialId>& children)
{
    BoundingVolume unit_bv(Box3::UNIT_BOX);
    std::shared_ptr<HydratedPortalZoneNodeAssembler> node_assembler = std::make_shared<HydratedPortalZoneNodeAssembler>(node_id);
    node_assembler->localTransform(Matrix4::IDENTITY);
    node_assembler->modelBound(unit_bv);
    node_assembler->topLevel(true);
    for (const auto& child : children)
    {
        node_assembler->child(child);
    }
    node_assembler->persist(node_id.name() + ".node", "DataPath");
    node_assembler->portalParentId(portal_id);

    return node_assembler;
}

std::shared_ptr<HydratedOutRegionNodeAssembler> SceneGraphMaker::makeHydratedOutsideRegion(const SpatialId& outside_id, const SpatialId& root_id, const SpatialId& portal_id, const SpatialId& inside_zone_id, const std::vector<SpatialId>& children)
{
    BoundingVolume unit_bv(Box3::UNIT_BOX);
    std::shared_ptr<HydratedOutRegionNodeAssembler> region_assembler = std::make_shared<HydratedOutRegionNodeAssembler>(outside_id);
    region_assembler->localTransform(Matrix4::IDENTITY);
    region_assembler->modelBound(unit_bv);
    region_assembler->topLevel(true);
    for (const auto& child : children)
    {
        region_assembler->child(child);
    }
    region_assembler->persist(outside_id.name() + ".node", "DataPath");
    region_assembler->parentId(root_id);

    std::shared_ptr<PortalAssembler> portal_assembler = std::make_shared<PortalAssembler>(portal_id);
    portal_assembler->localTransform(Matrix4::IDENTITY);
    portal_assembler->modelBound(PrimitiveMeshMaker::getDoorBound());
    portal_assembler->adjacentNodeId(inside_zone_id);
    portal_assembler->isOpen(true);
    portal_assembler->persist(portal_id.name() + ".portal", "DataPath");
    region_assembler->child(portal_assembler);
    return region_assembler;
}

GenericDto SceneGraphMaker::makeSceneGraph(const SpatialId& root_id, const std::shared_ptr<HydratedOutRegionNodeAssembler>& outside_region, const SpatialId
    & portal_id, const std::shared_ptr<HydratedPortalZoneNodeAssembler>& inside_zone)
{
    BoundingVolume root_bv(Box3::UNIT_BOX);
    std::shared_ptr<PortalManagementNodeAssembler> root_assembler = std::make_shared<PortalManagementNodeAssembler>(root_id);
    root_assembler->localTransform(Matrix4::IDENTITY);
    root_assembler->modelBound(root_bv);
    root_assembler->topLevel(true);
    //root_assembler->child(inside_zone);
    root_assembler->child(outside_region);
    root_assembler->persist(root_id.name() + ".node", "DataPath");
    root_assembler->outsideRegionId(outside_region->id());

    return root_assembler->assemble();
}
