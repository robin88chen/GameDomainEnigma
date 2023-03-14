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
    node_dto.ChildNames() = { "floor", "door" };
    PawnDto floor_dto;
    MeshPrimitiveDto mesh_dto = PrimitiveMeshMaker::MakeMeshPrimitiveDto("floor", "floor", "basic_floor_mesh", "fx/basic_floor_mesh.efx", "image/du011.png", "floor_tex", "DiffuseMap");
    floor_dto.TheFactoryDesc() = FactoryDesc(Pawn::TYPE_RTTI.GetName());
    floor_dto.Name() = "floor";
    floor_dto.LocalTransform() = Matrix4::IDENTITY;
    floor_dto.WorldTransform() = node_dto.WorldTransform() * floor_dto.LocalTransform();
    floor_dto.ModelBound() = PrimitiveMeshMaker::GetFloorBound().SerializeDto().ToGenericDto();
    floor_dto.WorldBound() = BoundingVolume::CreateFromTransform(PrimitiveMeshMaker::GetFloorBound(), floor_dto.WorldTransform()).SerializeDto().ToGenericDto();
    floor_dto.ThePrimitive() = mesh_dto.ToGenericDto();
    floor_dto.SpatialFlag() = static_cast<unsigned>(Spatial::SpatialBit::Spatial_BelongToParent | Spatial::Spatial_Unlit);
    PawnDto door_dto;
    mesh_dto = PrimitiveMeshMaker::MakeMeshPrimitiveDto("door", "door", "basic_floor_mesh", "fx/basic_floor_mesh.efx", "earth.png", "door_tex", "DiffuseMap");
    door_dto.TheFactoryDesc() = FactoryDesc(Pawn::TYPE_RTTI.GetName());
    door_dto.Name() = "door";
    door_dto.LocalTransform() = Matrix4::IDENTITY;
    door_dto.WorldTransform() = node_dto.WorldTransform() * door_dto.LocalTransform();
    door_dto.ModelBound() = PrimitiveMeshMaker::GetDoorBound().SerializeDto().ToGenericDto();
    door_dto.WorldBound() = BoundingVolume::CreateFromTransform(PrimitiveMeshMaker::GetDoorBound(), door_dto.WorldTransform()).SerializeDto().ToGenericDto();
    door_dto.ThePrimitive() = mesh_dto.ToGenericDto();
    door_dto.SpatialFlag() = static_cast<unsigned>(Spatial::SpatialBit::Spatial_BelongToParent | Spatial::Spatial_Unlit);

    return { node_dto.ToGenericDto(), floor_dto.ToGenericDto(), door_dto.ToGenericDto() };
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
