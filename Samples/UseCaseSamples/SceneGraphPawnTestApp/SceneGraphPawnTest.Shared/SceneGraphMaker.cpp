#include "SceneGraphMaker.h"
#include "MathLib/Matrix4.h"
#include "SceneGraph/Pawn.h"
#include "CubeGeometryMaker.h"
#include "SceneGraph/Spatial.h"
#include "SceneGraph/Node.h"
#include "SceneGraph/NodeAssembler.h"
#include "SceneGraph/PawnAssembler.h"

using namespace Enigma::SceneGraph;
using namespace Enigma::Frameworks;
using namespace Enigma::MathLib;
using namespace Enigma::Engine;
using namespace Enigma::Primitives;

Enigma::Engine::GenericDto SceneGraphMaker::makeSceneGraph(const SpatialId& id, const PrimitiveId& primitive_id, const Enigma::SceneGraph::SpatialId& pawn_id, const Enigma::SceneGraph::SpatialId& stillpawn_id)
{
    BoundingVolume root_bv(Box3::UNIT_BOX);
    std::shared_ptr<NodeAssembler> root_assembler = std::make_shared<NodeAssembler>(id);
    root_assembler->localTransform(Matrix4::IDENTITY);
    root_assembler->worldTransform(Matrix4::IDENTITY);
    root_assembler->modelBound(root_bv);
    root_assembler->topLevel(true);
    root_assembler->spatialFlags(Spatial::Spatial_Unlit);
    root_assembler->factory(FactoryDesc(Node::TYPE_RTTI.getName()).ClaimAsNative(id.name() + ".node@DataPath"));
    //root_dto.ChildNames() = { "child1", "child2" };
    BoundingVolume child_bv(Box3::UNIT_BOX);
    SpatialId child1_id("child1", Node::TYPE_RTTI);
    std::shared_ptr<NodeAssembler> child1_assembler = std::make_shared<NodeAssembler>(child1_id);
    child1_assembler->localTransform(Matrix4::IDENTITY);
    //child1_assembler->parentWorldTransform(root_assembler->worldTransform());
    child1_assembler->modelBound(child_bv);
    child1_assembler->spatialFlags(Spatial::Spatial_Unlit);
    //child1_assembler->parentId(id);
    //child1_dto.ChildNames() = { "still_pawn" };
    SpatialId child2_id("child2", Node::TYPE_RTTI);
    std::shared_ptr<NodeAssembler> child2_assembler = std::make_shared<NodeAssembler>(child2_id);
    child2_assembler->localTransform(Matrix4::MakeTranslateTransform(Vector3(2.0f, 0.0f, 0.0f)));
    child2_assembler->modelBound(child_bv);
    child2_assembler->spatialFlags(Spatial::Spatial_Unlit);
    //child2_assembler->parentId(id);
    //child2_dto.ChildNames() = { "pawn" };
    std::shared_ptr<PawnAssembler> pawn_assembler = std::make_shared<PawnAssembler>(pawn_id);
    pawn_assembler->primitiveId(primitive_id);
    pawn_assembler->localTransform(Matrix4::IDENTITY);
    pawn_assembler->modelBound(CubeGeometryMaker::getGeometryBound());
    std::shared_ptr<PawnAssembler> still_assembler = std::make_shared<PawnAssembler>(stillpawn_id);
    still_assembler->primitiveId(primitive_id.next());
    still_assembler->localTransform(Matrix4::IDENTITY);
    still_assembler->modelBound(CubeGeometryMaker::getGeometryBound());
    //ModelPrimitiveDto model_dto = SkinMeshModelMaker::MakeModelPrimitiveDto("test_model", "test_geometry");
    child1_assembler->child(still_assembler);
    child2_assembler->child(pawn_assembler);
    root_assembler->child(child1_assembler);
    root_assembler->child(child2_assembler);

    return root_assembler->assemble();
}
