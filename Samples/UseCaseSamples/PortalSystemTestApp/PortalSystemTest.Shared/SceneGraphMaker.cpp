#include "SceneGraphMaker.h"

#include <FileSystem/FileSystem.h>
#include <GameEngine/BoundingVolume.h>
#include <Gateways/DtoJsonGateway.h>
#include <Platforms/PlatformLayer.h>

#include "SceneGraph/SceneGraphDtos.h"
#include <SceneGraph/CameraFrustumDtos.h>
#include "SceneGraph/Node.h"
#include "PrimitiveMeshMaker.h"
#include "SceneGraph/Pawn.h"

using namespace Enigma::Engine;
using namespace Enigma::MathLib;
using namespace Enigma::SceneGraph;
using namespace Enigma::Renderer;
using namespace Enigma::FileSystem;

std::vector<Enigma::Engine::GenericDto> SceneGraphMaker::MakeFloorDtos(const std::string& node_name)
{
    BoundingVolume unit_bv(Box3::UNIT_BOX);
    NodeDto node_dto;
    node_dto.IsTopLevel() = true;
    node_dto.TheFactoryDesc() = FactoryDesc(Node::TYPE_RTTI.GetName());
    node_dto.Name() = node_name;
    node_dto.LocalTransform() = Matrix4::IDENTITY;
    node_dto.WorldTransform() = node_dto.LocalTransform();
    node_dto.ModelBound() = unit_bv.SerializeDto().ToGenericDto();
    node_dto.WorldBound() = unit_bv.SerializeDto().ToGenericDto();
    node_dto.SpatialFlag() = static_cast<unsigned>(Spatial::SpatialBit::Spatial_BelongToParent | Spatial::Spatial_Unlit);
    node_dto.ChildNames() = { "floor" };
    PawnDto pawn_dto;
    MeshPrimitiveDto mesh_dto = PrimitiveMeshMaker::MakeMeshPrimitiveDto("floor", "floor", "basic_floor_mesh", "fx/basic_floor_mesh.efx", "image/du011.png", "floor_tex", "DiffuseMap");
    pawn_dto.TheFactoryDesc() = FactoryDesc(Pawn::TYPE_RTTI.GetName());
    pawn_dto.Name() = "floor";
    pawn_dto.LocalTransform() = Matrix4::IDENTITY;
    pawn_dto.WorldTransform() = node_dto.WorldTransform() * pawn_dto.LocalTransform();
    pawn_dto.ModelBound() = PrimitiveMeshMaker::GetFloorBound().SerializeDto().ToGenericDto();
    pawn_dto.WorldBound() = BoundingVolume::CreateFromTransform(PrimitiveMeshMaker::GetFloorBound(), pawn_dto.WorldTransform()).SerializeDto().ToGenericDto();
    pawn_dto.ThePrimitive() = mesh_dto.ToGenericDto();
    pawn_dto.SpatialFlag() = static_cast<unsigned>(Spatial::SpatialBit::Spatial_BelongToParent | Spatial::Spatial_Unlit);

    return { node_dto.ToGenericDto(), pawn_dto.ToGenericDto() };
}

void SceneGraphMaker::MakeSavedFloorNode(const std::string& node_name)
{
    auto dtos = MakeFloorDtos(node_name);
    std::string json = Enigma::Gateways::DtoJsonGateway::Serialize(dtos);
    IFilePtr iFile = FileSystem::Instance()->OpenFile(Enigma::FileSystem::Filename(node_name + ".node@DataPath"), "w+b");
    if (FATAL_LOG_EXPR(!iFile)) return;
    iFile->Write(0, convert_to_buffer(json));
    FileSystem::Instance()->CloseFile(iFile);
}
