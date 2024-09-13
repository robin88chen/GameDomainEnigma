#include "SceneGraphMaker.h"
#include "SceneGraph/SceneGraphCommands.h"
#include "SceneGraph/PawnAssembler.h"
#include "SceneGraph/VisibilityManagedNode.h"
#include "SceneGraph/VisibilityManagedNodeAssembler.h"
#include "SceneGraph/Pawn.h"
#include "CubeGeometryMaker.h"
#include "Gateways/DtoJsonGateway.h"
#include "FileSystem/FileSystem.h"
#include "SceneGraph/SceneGraphQueries.h"
#include "SceneGraph/SceneGraphAssemblers.h"

using namespace Enigma::SceneGraph;
using namespace Enigma::Frameworks;
using namespace Enigma::Engine;
using namespace Enigma::MathLib;
using namespace Enigma::Gateways;
using namespace Enigma::FileSystem;

void SceneGraphMaker::makePawn(const Enigma::SceneGraph::SpatialId& id, const Enigma::Primitives::PrimitiveId& primitive_id)
{
    auto pawn = std::dynamic_pointer_cast<Pawn>(std::make_shared<QuerySpatial>(id)->dispatch());
    if (!pawn)
    {
        std::shared_ptr<PawnAssembler> pawn_assembler = std::make_shared<PawnAssembler>(id);
        pawn_assembler->primitiveId(primitive_id);
        pawn_assembler->localTransform(Matrix4::IDENTITY);
        pawn_assembler->modelBound(CubeGeometryMaker::getGeometryBound());
        pawn_assembler->topLevel(true);
        pawn_assembler->persist(id.name() + ".pawn", "DataPath");
        pawn = std::dynamic_pointer_cast<Pawn>(std::make_shared<RequestSpatialConstitution>(id, pawn_assembler->assemble())->dispatch());
        if (pawn)
        {
            std::make_shared<PutSpatial>(id, pawn)->execute();
        }
    }
}

GenericDto SceneGraphMaker::makeHydratedVisibilityNode(const SpatialId& id, const SpatialId& parent_id, const std::vector<SpatialId>& children)
{
    BoundingVolume unit_bv(Box3::UNIT_BOX);
    std::shared_ptr<HydratedVisibilityManagedNodeAssembler> visibility_assembler = std::make_shared<HydratedVisibilityManagedNodeAssembler>(id);
    visibility_assembler->factory(FactoryDesc(VisibilityManagedNode::TYPE_RTTI));
    visibility_assembler->persist(id.name() + ".node", "DataPath");
    visibility_assembler->localTransform(Matrix4::MakeTranslateTransform(Vector3(2.0f, 0.0f, 0.0f)));
    visibility_assembler->modelBound(unit_bv);
    visibility_assembler->topLevel(true);
    visibility_assembler->spatialFlags(Spatial::Spatial_Unlit);
    for (const auto& child : children)
    {
        visibility_assembler->child(child);
    }
    visibility_assembler->parentId(parent_id);
    return visibility_assembler->assemble();
}

GenericDto SceneGraphMaker::makeSceneGraph(const SpatialId& root_id, const SpatialId& visibility_node_id)
{
    BoundingVolume root_bv(Box3::UNIT_BOX);
    std::shared_ptr<NodeAssembler> root_assembler = std::make_shared<NodeAssembler>(root_id);
    root_assembler->localTransform(Matrix4::IDENTITY);
    root_assembler->worldTransform(Matrix4::IDENTITY);
    root_assembler->modelBound(root_bv);
    root_assembler->topLevel(true);
    root_assembler->spatialFlags(Spatial::Spatial_Unlit);
    root_assembler->factory(FactoryDesc(Node::TYPE_RTTI.getName()).ClaimAsNative(root_id.name() + ".node@DataPath"));
    //root_dto.ChildNames() = { "child1", lazy_node_name };
    BoundingVolume child_bv(Box3::UNIT_BOX);
    SpatialId child1_id("child1", Node::TYPE_RTTI);
    std::shared_ptr<NodeAssembler> child1_assembler = std::make_shared<NodeAssembler>(child1_id);
    child1_assembler->localTransform(Matrix4::IDENTITY);
    //child1_assembler->parentWorldTransform(root_assembler->worldTransform());
    child1_assembler->modelBound(child_bv);
    child1_assembler->spatialFlags(Spatial::Spatial_Unlit);

    std::shared_ptr<DehydratedVisibilityManagedNodeAssembler> visibility_assembler = std::make_shared<DehydratedVisibilityManagedNodeAssembler>(visibility_node_id);
    visibility_assembler->factory(FactoryDesc(VisibilityManagedNode::TYPE_RTTI));
    visibility_assembler->persist(visibility_node_id.name() + ".node", "DataPath");
    visibility_assembler->localTransform(Matrix4::MakeTranslateTransform(Vector3(2.0f, 0.0f, 0.0f)));
    visibility_assembler->modelBound(child_bv);
    visibility_assembler->spatialFlags(Spatial::Spatial_Unlit);
    root_assembler->child(child1_assembler);
    root_assembler->child(visibility_assembler);
    return root_assembler->assemble();
}
