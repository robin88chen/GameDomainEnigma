#include "SceneGraphMaker.h"
#include "Frameworks/ServiceManager.h"
#include "MathLib/Matrix4.h"
#include "SceneGraph/Pawn.h"
#include "SkinMeshModelMaker.h"
#include "SceneGraph/SceneGraphDtos.h"
#include "CubeGeometryMaker.h"
#include "SceneGraph/Spatial.h"
#include "SceneGraph/Node.h"

using namespace Enigma::SceneGraph;
using namespace Enigma::Frameworks;
using namespace Enigma::MathLib;
using namespace Enigma::Engine;
using namespace Enigma::Renderer;

std::vector<Enigma::Engine::GenericDto> SceneGraphMaker::MakeSceneGraphDtos()
{
    BoundingVolume root_bv(Box3::UNIT_BOX);
    NodeDto root_dto;
    root_dto.TheFactoryDesc() = FactoryDesc(Node::TYPE_RTTI.GetName());
    root_dto.Name() = "scene_root";
    root_dto.LocalTransform() = Matrix4::IDENTITY;
    root_dto.WorldTransform() = Matrix4::IDENTITY;
    root_dto.ModelBound() = root_bv.SerializeDto().ToGenericDto();
    root_dto.WorldBound() = root_bv.SerializeDto().ToGenericDto();
    root_dto.IsTopLevel() = true;
    root_dto.SpatialFlag() = static_cast<unsigned>(Spatial::SpatialBit::Spatial_BelongToParent | Spatial::Spatial_Unlit);
    root_dto.ChildNames() = { "child1", "child2" };
    BoundingVolume child_bv(Box3::UNIT_BOX);
    NodeDto child1_dto;
    child1_dto.TheFactoryDesc() = FactoryDesc(Node::TYPE_RTTI.GetName());
    child1_dto.Name() = "child1";
    child1_dto.LocalTransform() = Matrix4::IDENTITY;
    child1_dto.WorldTransform() = root_dto.WorldTransform() * child1_dto.LocalTransform();
    child1_dto.ModelBound() = child_bv.SerializeDto().ToGenericDto();
    child1_dto.WorldBound() = BoundingVolume::CreateFromTransform(child_bv, child1_dto.WorldTransform()).SerializeDto().ToGenericDto();
    child1_dto.SpatialFlag() = static_cast<unsigned>(Spatial::SpatialBit::Spatial_BelongToParent | Spatial::Spatial_Unlit);
    NodeDto child2_dto;
    child2_dto.TheFactoryDesc() = FactoryDesc(Node::TYPE_RTTI.GetName());
    child2_dto.Name() = "child2";
    child2_dto.LocalTransform() = Matrix4::MakeTranslateTransform(Vector3(2.0f, 0.0f, 0.0f));
    child2_dto.WorldTransform() = root_dto.WorldTransform() * child2_dto.LocalTransform();
    child2_dto.ModelBound() = child_bv.SerializeDto().ToGenericDto();
    child2_dto.WorldBound() = BoundingVolume::CreateFromTransform(child_bv, child2_dto.WorldTransform()).SerializeDto().ToGenericDto();
    child2_dto.SpatialFlag() = static_cast<unsigned>(Spatial::SpatialBit::Spatial_BelongToParent | Spatial::Spatial_Unlit);
    child2_dto.ChildNames() = { "pawn" };
    PawnDto pawn_dto;
    ModelPrimitiveDto model_dto = SkinMeshModelMaker::MakeModelPrimitiveDto("test_model", "test_geometry");
    pawn_dto.TheFactoryDesc() = FactoryDesc(Pawn::TYPE_RTTI.GetName());
    pawn_dto.Name() = "pawn";
    pawn_dto.LocalTransform() = Matrix4::IDENTITY;
    pawn_dto.WorldTransform() = child2_dto.WorldTransform() * pawn_dto.LocalTransform();
    pawn_dto.ModelBound() = CubeGeometryMaker::GetGeometryBound().SerializeDto().ToGenericDto();
    pawn_dto.WorldBound() = BoundingVolume::CreateFromTransform(CubeGeometryMaker::GetGeometryBound(), pawn_dto.WorldTransform()).SerializeDto().ToGenericDto();
    pawn_dto.ThePrimitive() = model_dto.ToGenericDto();
    pawn_dto.SpatialFlag() = static_cast<unsigned>(Spatial::SpatialBit::Spatial_BelongToParent | Spatial::Spatial_Unlit);

    return { root_dto.ToGenericDto(), child1_dto.ToGenericDto(), child2_dto.ToGenericDto(), pawn_dto.ToGenericDto() };
}
