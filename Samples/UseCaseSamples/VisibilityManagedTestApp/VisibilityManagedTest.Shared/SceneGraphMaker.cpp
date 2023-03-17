#include "SceneGraphMaker.h"
#include "SceneGraph/SceneGraphDtos.h"
#include "SceneGraph/VisibilityManagedNode.h"
#include "SceneGraph/Pawn.h"
#include "SkinMeshModelMaker.h"
#include "Renderer/RenderablePrimitiveDtos.h"
#include "CubeGeometryMaker.h"
#include "Gateways/DtoJsonGateway.h"
#include "FileSystem/FileSystem.h"
#include "Platforms/PlatformLayer.h"

using namespace Enigma::SceneGraph;
using namespace Enigma::Frameworks;
using namespace Enigma::Engine;
using namespace Enigma::MathLib;
using namespace Enigma::Renderer;
using namespace Enigma::Gateways;
using namespace Enigma::FileSystem;

void SceneGraphMaker::MakeSavedVisibilityNode(const std::string& name)
{
    BoundingVolume unit_bv(Box3::UNIT_BOX);
    VisibilityManagedNodeDto visibility_managed_node_dto;
    visibility_managed_node_dto.IsTopLevel() = true;
    visibility_managed_node_dto.TheFactoryDesc() = FactoryDesc(VisibilityManagedNode::TYPE_RTTI.GetName());
    visibility_managed_node_dto.Name() = name;
    visibility_managed_node_dto.LocalTransform() = Matrix4::MakeTranslateTransform(Vector3(2.0f, 0.0f, 0.0f));
    visibility_managed_node_dto.WorldTransform() = visibility_managed_node_dto.LocalTransform();
    visibility_managed_node_dto.ModelBound() = unit_bv.SerializeDto().ToGenericDto();
    visibility_managed_node_dto.WorldBound() = BoundingVolume::CreateFromTransform(unit_bv, visibility_managed_node_dto.WorldTransform()).SerializeDto().ToGenericDto();
    visibility_managed_node_dto.SpatialFlag() = static_cast<unsigned>(Spatial::SpatialBit::Spatial_BelongToParent | Spatial::Spatial_Unlit);
    visibility_managed_node_dto.ChildNames() = { "pawn" };
    PawnDto pawn_dto;
    ModelPrimitiveDto model_dto = SkinMeshModelMaker::MakeModelPrimitiveDto("test_model", "test_geometry");
    pawn_dto.TheFactoryDesc() = FactoryDesc(Pawn::TYPE_RTTI.GetName());
    pawn_dto.Name() = "pawn";
    pawn_dto.LocalTransform() = Matrix4::IDENTITY;
    pawn_dto.WorldTransform() = visibility_managed_node_dto.WorldTransform() * pawn_dto.LocalTransform();
    pawn_dto.ModelBound() = CubeGeometryMaker::GetGeometryBound().SerializeDto().ToGenericDto();
    pawn_dto.WorldBound() = BoundingVolume::CreateFromTransform(CubeGeometryMaker::GetGeometryBound(), pawn_dto.WorldTransform()).SerializeDto().ToGenericDto();
    pawn_dto.ThePrimitive() = model_dto.ToGenericDto();
    pawn_dto.SpatialFlag() = static_cast<unsigned>(Spatial::SpatialBit::Spatial_BelongToParent | Spatial::Spatial_Unlit);

    std::vector<GenericDto> dtos = { visibility_managed_node_dto.ToGenericDto(), pawn_dto.ToGenericDto() };
    std::string json = DtoJsonGateway::Serialize(dtos);
    IFilePtr iFile = FileSystem::Instance()->OpenFile(Filename(name + ".node@DataPath"), "w+b");
    if (FATAL_LOG_EXPR(!iFile)) return;
    iFile->Write(0, convert_to_buffer(json));
    FileSystem::Instance()->CloseFile(iFile);
}
std::vector<GenericDto> SceneGraphMaker::MakeSceneGraphDtos(const std::string& visibility_node_name)
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
    root_dto.ChildNames() = { "child1", visibility_node_name };
    BoundingVolume child_bv(Box3::UNIT_BOX);
    NodeDto child1_dto;
    child1_dto.TheFactoryDesc() = FactoryDesc(Node::TYPE_RTTI.GetName());
    child1_dto.Name() = "child1";
    child1_dto.LocalTransform() = Matrix4::IDENTITY;
    child1_dto.WorldTransform() = root_dto.WorldTransform() * child1_dto.LocalTransform();
    child1_dto.ModelBound() = child_bv.SerializeDto().ToGenericDto();
    child1_dto.WorldBound() = BoundingVolume::CreateFromTransform(child_bv, child1_dto.WorldTransform()).SerializeDto().ToGenericDto();
    child1_dto.SpatialFlag() = static_cast<unsigned>(Spatial::SpatialBit::Spatial_BelongToParent | Spatial::Spatial_Unlit);
    VisibilityManagedNodeDto visibility_managed_node_dto;
    visibility_managed_node_dto.TheFactoryDesc() = FactoryDesc(VisibilityManagedNode::TYPE_RTTI.GetName());
    visibility_managed_node_dto.TheFactoryDesc().ClaimAsDeferred(visibility_node_name + ".node", "DataPath");
    visibility_managed_node_dto.Name() = visibility_node_name;
    visibility_managed_node_dto.LocalTransform() = Matrix4::MakeTranslateTransform(Vector3(2.0f, 0.0f, 0.0f));
    visibility_managed_node_dto.WorldTransform() = root_dto.WorldTransform() * visibility_managed_node_dto.LocalTransform();
    visibility_managed_node_dto.ModelBound() = child_bv.SerializeDto().ToGenericDto();
    visibility_managed_node_dto.WorldBound() = BoundingVolume::CreateFromTransform(child_bv, visibility_managed_node_dto.WorldTransform()).SerializeDto().ToGenericDto();
    visibility_managed_node_dto.SpatialFlag() = static_cast<unsigned>(Spatial::SpatialBit::Spatial_BelongToParent | Spatial::Spatial_Unlit);

    std::vector<GenericDto> dtos = { root_dto.ToGenericDto(), child1_dto.ToGenericDto(), visibility_managed_node_dto.ToGenericDto() };
    std::string json = DtoJsonGateway::Serialize(dtos);
    IFilePtr iFile = FileSystem::Instance()->OpenFile(Filename("scene_root.node@DataPath"), "w+b");
    if (FATAL_LOG_EXPR(!iFile)) return dtos;
    iFile->Write(0, convert_to_buffer(json));
    FileSystem::Instance()->CloseFile(iFile);
    return dtos;
}
