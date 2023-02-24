#include "SceneGraphMaker.h"
#include "SceneGraph/SceneGraphDtos.h"
#include "SceneGraph/LazyNode.h"
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

void SceneGraphMaker::MakeSavedLazyNode(const std::string& name)
{
    BoundingVolume unit_bv(Box3::UNIT_BOX);
    LazyNodeDto lazy_dto;
    lazy_dto.IsTopLevel() = true;
    lazy_dto.TheFactoryDesc() = FactoryDesc(LazyNode::TYPE_RTTI.GetName());
    lazy_dto.Name() = name;
    lazy_dto.LocalTransform() = Matrix4::MakeTranslateTransform(Vector3(2.0f, 0.0f, 0.0f));
    lazy_dto.WorldTransform() = lazy_dto.LocalTransform();
    lazy_dto.ModelBound() = unit_bv.SerializeDto().ToGenericDto();
    lazy_dto.WorldBound() = BoundingVolume::CreateFromTransform(unit_bv, lazy_dto.WorldTransform()).SerializeDto().ToGenericDto();
    lazy_dto.SpatialFlag() = static_cast<unsigned>(Spatial::SpatialBit::Spatial_BelongToParent | Spatial::Spatial_Unlit);
    lazy_dto.ChildNames() = { "pawn" };
    PawnDto pawn_dto;
    ModelPrimitiveDto model_dto = SkinMeshModelMaker::MakeModelPrimitiveDto("test_model", "test_geometry");
    pawn_dto.TheFactoryDesc() = FactoryDesc(Pawn::TYPE_RTTI.GetName());
    pawn_dto.Name() = "pawn";
    pawn_dto.LocalTransform() = Matrix4::IDENTITY;
    pawn_dto.WorldTransform() = lazy_dto.WorldTransform() * pawn_dto.LocalTransform();
    pawn_dto.ModelBound() = CubeGeometryMaker::GetGeometryBound().SerializeDto().ToGenericDto();
    pawn_dto.WorldBound() = BoundingVolume::CreateFromTransform(CubeGeometryMaker::GetGeometryBound(), pawn_dto.WorldTransform()).SerializeDto().ToGenericDto();
    pawn_dto.ThePrimitive() = model_dto.ToGenericDto();
    pawn_dto.SpatialFlag() = static_cast<unsigned>(Spatial::SpatialBit::Spatial_BelongToParent | Spatial::Spatial_Unlit);

    std::vector<GenericDto> dtos = { lazy_dto.ToGenericDto(), pawn_dto.ToGenericDto() };
    std::string json = DtoJsonGateway::Serialize(dtos);
    IFilePtr iFile = FileSystem::Instance()->OpenFile(Filename(name + ".node@DataPath"), "w+b");
    if (FATAL_LOG_EXPR(!iFile)) return;
    iFile->Write(0, convert_to_buffer(json));
    FileSystem::Instance()->CloseFile(iFile);
}
std::vector<GenericDto> SceneGraphMaker::MakeSceneGraphDtos()
{
    return {};
}
