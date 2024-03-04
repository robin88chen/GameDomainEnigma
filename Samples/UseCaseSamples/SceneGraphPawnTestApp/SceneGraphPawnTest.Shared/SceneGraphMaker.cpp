#include "SceneGraphMaker.h"
#include "MathLib/Matrix4.h"
#include "SceneGraph/Pawn.h"
#include "SceneGraph/SceneGraphDtos.h"
#include "CubeGeometryMaker.h"
#include "SceneGraph/Spatial.h"
#include "SceneGraph/Node.h"
#include "SceneGraph/SceneGraphAssemblers.h"

using namespace Enigma::SceneGraph;
using namespace Enigma::Frameworks;
using namespace Enigma::MathLib;
using namespace Enigma::Engine;
using namespace Enigma::Primitives;

Enigma::Engine::GenericDto SceneGraphMaker::makeSceneGraph(const SpatialId& id, const PrimitiveId& primitive_id, const Enigma::SceneGraph::SpatialId& pawn_id, const Enigma::SceneGraph::SpatialId& stillpawn_id)
{
    BoundingVolume root_bv(Box3::UNIT_BOX);
    NodeDto root_dto;
    root_dto.id() = id;
    root_dto.name() = id.name();
    root_dto.localTransform() = Matrix4::IDENTITY;
    root_dto.worldTransform() = Matrix4::IDENTITY;
    root_dto.modelBound() = root_bv.serializeDto().toGenericDto();
    root_dto.worldBound() = root_bv.serializeDto().toGenericDto();
    root_dto.isTopLevel() = true;
    root_dto.spatialFlag() = static_cast<unsigned>(Spatial::Spatial_Unlit);
    //root_dto.ChildNames() = { "child1", "child2" };
    BoundingVolume child_bv(Box3::UNIT_BOX);
    NodeDto child1_dto;
    child1_dto.id() = SpatialId("child1", Node::TYPE_RTTI);
    child1_dto.name() = "child1";
    child1_dto.localTransform() = Matrix4::IDENTITY;
    child1_dto.worldTransform() = root_dto.worldTransform() * child1_dto.localTransform();
    child1_dto.modelBound() = child_bv.serializeDto().toGenericDto();
    child1_dto.worldBound() = BoundingVolume::CreateFromTransform(child_bv, child1_dto.worldTransform()).serializeDto().toGenericDto();
    child1_dto.spatialFlag() = static_cast<unsigned>(Spatial::Spatial_Unlit);
    child1_dto.parentId() = root_dto.id();
    //child1_dto.ChildNames() = { "still_pawn" };
    NodeDto child2_dto;
    child2_dto.id() = SpatialId("child2", Node::TYPE_RTTI);
    child2_dto.name() = "child2";
    child2_dto.localTransform() = Matrix4::MakeTranslateTransform(Vector3(2.0f, 0.0f, 0.0f));
    child2_dto.worldTransform() = root_dto.worldTransform() * child2_dto.localTransform();
    child2_dto.modelBound() = child_bv.serializeDto().toGenericDto();
    child2_dto.worldBound() = BoundingVolume::CreateFromTransform(child_bv, child2_dto.worldTransform()).serializeDto().toGenericDto();
    child2_dto.spatialFlag() = static_cast<unsigned>(Spatial::Spatial_Unlit);
    child2_dto.parentId() = root_dto.id();
    //child2_dto.ChildNames() = { "pawn" };
    PawnDto pawn_dto = PawnAssembler(pawn_id).primitive(primitive_id).localTransform(Matrix4::IDENTITY).worldTransform(child2_dto.worldTransform()).modelBound(CubeGeometryMaker::getGeometryBound()).toPawnDto();
    pawn_dto.parentId() = child2_dto.id();
    PawnDto stillpawn_dto = PawnAssembler(stillpawn_id).primitive(primitive_id.next()).localTransform(Matrix4::IDENTITY).worldTransform(child1_dto.worldTransform()).modelBound(CubeGeometryMaker::getGeometryBound()).toPawnDto();
    stillpawn_dto.parentId() = child1_dto.id();
    //ModelPrimitiveDto model_dto = SkinMeshModelMaker::MakeModelPrimitiveDto("test_model", "test_geometry");
    child1_dto.children().emplace_back(NodeDto::ChildDto{ stillpawn_dto.id(), stillpawn_dto.toGenericDto() });
    child2_dto.children().emplace_back(NodeDto::ChildDto{ pawn_dto.id(), pawn_dto.toGenericDto() });
    root_dto.children().emplace_back(NodeDto::ChildDto{ child1_dto.id(), child1_dto.toGenericDto() });
    root_dto.children().emplace_back(NodeDto::ChildDto{ child2_dto.id(), child2_dto.toGenericDto() });

    return root_dto.toGenericDto();
}
