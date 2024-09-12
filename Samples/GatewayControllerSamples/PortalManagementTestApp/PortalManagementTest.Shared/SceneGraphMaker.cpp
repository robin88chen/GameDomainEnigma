#include "SceneGraphMaker.h"
#include "FileSystem/FileSystem.h"
#include "GameEngine/BoundingVolume.h"
#include "SceneGraph/SceneGraphQueries.h"
#include "SceneGraph/Node.h"
#include "PrimitiveMeshMaker.h"
#include "SceneGraph/Pawn.h"
#include "SceneGraph/PawnAssembler.h"
#include "SceneGraph/PortalManagementNodeAssembler.h"
#include "SceneGraph/PortalZoneNode.h"
#include "SceneGraph/PortalAssembler.h"
#include "SceneGraph/SceneGraphCommands.h"

using namespace Enigma::Engine;
using namespace Enigma::MathLib;
using namespace Enigma::SceneGraph;
using namespace Enigma::FileSystem;

GenericDto SceneGraphMaker::makePawn(const SpatialId& id, const Enigma::Primitives::PrimitiveId& primitive_id, const
    BoundingVolume& geometry_bound)
{
    std::shared_ptr<PawnAssembler> assembler = std::make_shared<PawnAssembler>(id);
    assembler->localTransform(Matrix4::IDENTITY);
    assembler->modelBound(geometry_bound);
    assembler->topLevel(true);
    assembler->primitiveId(primitive_id);
    assembler->persist(id.name() + ".pawn", "DataPath");
    return assembler->assemble();
}

std::tuple<std::shared_ptr<HydratedPortalZoneNodeAssembler>, std::shared_ptr<DehydratedPortalZoneNodeAssembler>> SceneGraphMaker::makeInsideZoneNode(const SpatialId& node_id, const SpatialId& portal_id, const std::vector<SpatialId>& children)
{
    BoundingVolume unit_bv(Box3::UNIT_BOX);
    std::shared_ptr<HydratedPortalZoneNodeAssembler> hydrated_assembler = std::make_shared<HydratedPortalZoneNodeAssembler>(node_id);
    std::shared_ptr<DehydratedPortalZoneNodeAssembler> dehydrated_assembler = std::make_shared<DehydratedPortalZoneNodeAssembler>(node_id);
    hydrated_assembler->localTransform(Matrix4::IDENTITY);
    dehydrated_assembler->localTransform(Matrix4::IDENTITY);
    hydrated_assembler->modelBound(unit_bv);
    dehydrated_assembler->modelBound(unit_bv);
    hydrated_assembler->topLevel(true);
    dehydrated_assembler->topLevel(true);
    for (const auto& child : children)
    {
        hydrated_assembler->child(child);
    }
    hydrated_assembler->persist(node_id.name() + ".node", "DataPath");
    dehydrated_assembler->persist(node_id.name() + ".node", "DataPath");
    hydrated_assembler->portalParentId(portal_id);
    dehydrated_assembler->portalParentId(portal_id);

    return { hydrated_assembler, dehydrated_assembler };
}

std::tuple<std::shared_ptr<HydratedOutRegionNodeAssembler>, std::shared_ptr<DehydratedOutRegionNodeAssembler>> SceneGraphMaker::makeOutsideRegion(const SpatialId& outside_id, const SpatialId& root_id, const SpatialId& portal_id, const SpatialId& inside_zone_id, const std::vector<SpatialId>& children)
{
    BoundingVolume unit_bv(Box3::UNIT_BOX);
    std::shared_ptr<HydratedOutRegionNodeAssembler> hydrated_assembler = std::make_shared<HydratedOutRegionNodeAssembler>(outside_id);
    std::shared_ptr<DehydratedOutRegionNodeAssembler> dehydrated_assembler = std::make_shared<DehydratedOutRegionNodeAssembler>(outside_id);

    hydrated_assembler->localTransform(Matrix4::IDENTITY);
    dehydrated_assembler->localTransform(Matrix4::IDENTITY);
    hydrated_assembler->modelBound(unit_bv);
    dehydrated_assembler->modelBound(unit_bv);
    hydrated_assembler->topLevel(true);
    dehydrated_assembler->topLevel(true);
    for (const auto& child : children)
    {
        hydrated_assembler->child(child);
    }
    hydrated_assembler->persist(outside_id.name() + ".node", "DataPath");
    dehydrated_assembler->persist(outside_id.name() + ".node", "DataPath");
    hydrated_assembler->parentId(root_id);
    dehydrated_assembler->parentId(root_id);

    std::shared_ptr<PortalAssembler> portal_assembler = std::make_shared<PortalAssembler>(portal_id);
    portal_assembler->localTransform(Matrix4::IDENTITY);
    portal_assembler->modelBound(PrimitiveMeshMaker::getDoorBound());
    portal_assembler->adjacentNodeId(inside_zone_id);
    portal_assembler->isOpen(true);
    portal_assembler->persist(portal_id.name() + ".portal", "DataPath");
    hydrated_assembler->child(portal_assembler);
    return { hydrated_assembler, dehydrated_assembler };
}

GenericDto SceneGraphMaker::makePortalManagementNode(const SpatialId& node_id, const std::shared_ptr<HydratedOutRegionNodeAssembler>& outside_region, const std::shared_ptr<HydratedPortalZoneNodeAssembler>& inside_zone)
{
    BoundingVolume root_bv(Box3::UNIT_BOX);
    std::shared_ptr<PortalManagementNodeAssembler> root_assembler = std::make_shared<PortalManagementNodeAssembler>(node_id);
    root_assembler->localTransform(Matrix4::IDENTITY);
    root_assembler->modelBound(root_bv);
    root_assembler->topLevel(true);
    //root_assembler->child(inside_zone);
    root_assembler->child(outside_region);
    root_assembler->persist(node_id.name() + ".node", "DataPath");
    root_assembler->outsideRegionId(outside_region->id());

    return root_assembler->assemble();
}
