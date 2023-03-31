#include "SceneGraphMaker.h"
#include "FileSystem/FileSystem.h"
#include "GameEngine/BoundingVolume.h"
#include "Gateways/DtoJsonGateway.h"
#include "Platforms/PlatformLayer.h"
#include "SceneGraph/SceneGraphDtos.h"
#include "SceneGraph/CameraFrustumDtos.h"
#include "SceneGraph/Node.h"
#include "PrimitiveMeshMaker.h"
#include "SceneGraph/Pawn.h"
#include "SceneGraph/PortalDtos.h"
#include "SceneGraph/PortalManagementNode.h"
#include "SceneGraph/PortalZoneNode.h"
#include "SceneGraph/Portal.h"

using namespace Enigma::Engine;
using namespace Enigma::MathLib;
using namespace Enigma::SceneGraph;
using namespace Enigma::Renderer;
using namespace Enigma::FileSystem;

std::vector<GenericDto> SceneGraphMaker::MakeFloorDtos(const std::string& node_name)
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
    node_dto.ChildNames() = { "floor", "door", "board" };
    PawnDto floor_dto = MakeFloorPawn(node_dto.WorldTransform());
    PawnDto door_dto = MakeDoorPawn(node_dto.WorldTransform());
    PawnDto board_dto = MakeBoardPawn(node_dto.WorldTransform());
    return { node_dto.ToGenericDto(), floor_dto.ToGenericDto(), door_dto.ToGenericDto(), board_dto.ToGenericDto() };
}

void SceneGraphMaker::MakeSavedFloorNode(const std::string& node_name)
{
    auto dtos = MakeFloorDtos(node_name);
    std::string json = Enigma::Gateways::DtoJsonGateway::Serialize(dtos);
    IFilePtr iFile = FileSystem::Instance()->OpenFile(Filename(node_name + ".node@DataPath"), "w+b");
    if (FATAL_LOG_EXPR(!iFile)) return;
    iFile->Write(0, convert_to_buffer(json));
    FileSystem::Instance()->CloseFile(iFile);
}

std::vector<Enigma::Engine::GenericDto> SceneGraphMaker::MakePortalSceneGraph(const std::string& node_name)
{
    BoundingVolume unit_bv(Box3::UNIT_BOX);
    PortalManagementNodeDto node_dto;
    node_dto.IsTopLevel() = true;
    node_dto.TheFactoryDesc() = FactoryDesc(PortalManagementNode::TYPE_RTTI.GetName());
    node_dto.Name() = node_name;
    node_dto.LocalTransform() = Matrix4::IDENTITY;
    node_dto.WorldTransform() = node_dto.LocalTransform();
    node_dto.ModelBound() = unit_bv.SerializeDto().ToGenericDto();
    node_dto.WorldBound() = unit_bv.SerializeDto().ToGenericDto();
    node_dto.SpatialFlag() = static_cast<unsigned>(Spatial::SpatialBit::Spatial_BelongToParent | Spatial::Spatial_Unlit);
    node_dto.OutsideZoneNodeName() = "outside_region";

    PortalZoneNodeDto outside_dto = MakeOutsideRegion(node_dto.OutsideZoneNodeName(), node_dto.WorldTransform());
    outside_dto.TheFactoryDesc().ClaimAsDeferred(outside_dto.Name() + ".node", "DataPath");
    outside_dto.IsTopLevel() = false;
    outside_dto.ChildNames() = {};

    return { node_dto.ToGenericDto(), outside_dto.ToGenericDto() };
}

void SceneGraphMaker::MakeSavedPortalSceneGraph(const std::string& node_name)
{
    auto dtos = MakePortalSceneGraph(node_name);
    std::string json = Enigma::Gateways::DtoJsonGateway::Serialize(dtos);
    IFilePtr iFile = FileSystem::Instance()->OpenFile(Filename(node_name + ".node@DataPath"), "w+b");
    if (FATAL_LOG_EXPR(!iFile)) return;
    iFile->Write(0, convert_to_buffer(json));
    FileSystem::Instance()->CloseFile(iFile);
}

PawnDto SceneGraphMaker::MakeFloorPawn(const Matrix4& parent_world)
{
    PawnDto floor_dto;
    MeshPrimitiveDto mesh_dto = PrimitiveMeshMaker::MakeMeshPrimitiveDto("floor", "floor", "basic_floor_mesh", "fx/basic_floor_mesh.efx", "image/du011.png", "floor_tex", "DiffuseMap");
    floor_dto.TheFactoryDesc() = FactoryDesc(Pawn::TYPE_RTTI.GetName());
    floor_dto.Name() = "floor";
    floor_dto.LocalTransform() = Matrix4::IDENTITY;
    floor_dto.WorldTransform() = parent_world * floor_dto.LocalTransform();
    floor_dto.ModelBound() = PrimitiveMeshMaker::GetFloorBound().SerializeDto().ToGenericDto();
    floor_dto.WorldBound() = BoundingVolume::CreateFromTransform(PrimitiveMeshMaker::GetFloorBound(), floor_dto.WorldTransform()).SerializeDto().ToGenericDto();
    floor_dto.ThePrimitive() = mesh_dto.ToGenericDto();
    floor_dto.SpatialFlag() = static_cast<unsigned>(Spatial::SpatialBit::Spatial_BelongToParent | Spatial::Spatial_Unlit);
    return floor_dto;
}

PawnDto SceneGraphMaker::MakeDoorPawn(const Matrix4& parent_world)
{
    PawnDto door_dto;
    MeshPrimitiveDto mesh_dto = PrimitiveMeshMaker::MakeMeshPrimitiveDto("door", "door", "basic_floor_mesh", "fx/basic_floor_mesh.efx", "earth.png", "door_tex", "DiffuseMap");
    door_dto.TheFactoryDesc() = FactoryDesc(Pawn::TYPE_RTTI.GetName());
    door_dto.Name() = "door";
    door_dto.LocalTransform() = Matrix4::IDENTITY;
    door_dto.WorldTransform() = parent_world * door_dto.LocalTransform();
    door_dto.ModelBound() = PrimitiveMeshMaker::GetDoorBound().SerializeDto().ToGenericDto();
    door_dto.WorldBound() = BoundingVolume::CreateFromTransform(PrimitiveMeshMaker::GetDoorBound(), door_dto.WorldTransform()).SerializeDto().ToGenericDto();
    door_dto.ThePrimitive() = mesh_dto.ToGenericDto();
    door_dto.SpatialFlag() = static_cast<unsigned>(Spatial::SpatialBit::Spatial_BelongToParent | Spatial::Spatial_Unlit);
    return door_dto;
}

PawnDto SceneGraphMaker::MakeBoardPawn(const Matrix4& parent_world)
{
    PawnDto board_dto;
    MeshPrimitiveDto mesh_dto = PrimitiveMeshMaker::MakeMeshPrimitiveDto("board", "board", "basic_floor_mesh", "fx/basic_floor_mesh.efx", "image/hga0010101.png", "board_tex", "DiffuseMap");
    board_dto.TheFactoryDesc() = FactoryDesc(Pawn::TYPE_RTTI.GetName());
    board_dto.Name() = "board";
    board_dto.LocalTransform() = Matrix4::IDENTITY;
    board_dto.WorldTransform() = parent_world * board_dto.LocalTransform();
    board_dto.ModelBound() = PrimitiveMeshMaker::GetBoardBound().SerializeDto().ToGenericDto();
    board_dto.WorldBound() = BoundingVolume::CreateFromTransform(PrimitiveMeshMaker::GetBoardBound(), board_dto.WorldTransform()).SerializeDto().ToGenericDto();
    board_dto.ThePrimitive() = mesh_dto.ToGenericDto();
    board_dto.SpatialFlag() = static_cast<unsigned>(Spatial::SpatialBit::Spatial_BelongToParent | Spatial::Spatial_Unlit);
    return board_dto;
}

Enigma::SceneGraph::PortalZoneNodeDto SceneGraphMaker::MakeOutsideRegion(const std::string name, const Enigma::MathLib::Matrix4& parent_world)
{
    BoundingVolume unit_bv(Box3::UNIT_BOX);
    PortalZoneNodeDto outside_dto;
    outside_dto.TheFactoryDesc() = FactoryDesc(PortalZoneNode::TYPE_RTTI.GetName());
    outside_dto.Name() = name;
    outside_dto.LocalTransform() = Matrix4::IDENTITY;
    outside_dto.WorldTransform() = parent_world * outside_dto.LocalTransform();
    outside_dto.ModelBound() = unit_bv.SerializeDto().ToGenericDto();
    outside_dto.WorldBound() = BoundingVolume::CreateFromTransform(unit_bv, outside_dto.WorldTransform()).SerializeDto().ToGenericDto();
    outside_dto.SpatialFlag() = static_cast<unsigned>(Spatial::SpatialBit::Spatial_BelongToParent | Spatial::Spatial_Unlit);
    outside_dto.ChildNames() = { "floor", "door", "inside_portal" };
    PawnDto floor_dto = MakeFloorPawn(outside_dto.WorldTransform());
    PawnDto door_dto = MakeDoorPawn(outside_dto.WorldTransform());
    PortalDto portal_dto = MakeDirectingInsidePortal(outside_dto.WorldTransform());
    PortalZoneNodeDto inside_dto = MakeInsideRegion("inside_region", outside_dto.WorldTransform());
    inside_dto.TheFactoryDesc().ClaimAsDeferred(inside_dto.Name() + ".node", "DataPath");
    inside_dto.IsTopLevel() = false;
    inside_dto.ChildNames() = {};

    outside_dto.IsTopLevel() = true;
    SaveLazyRegion(outside_dto.Name(), { outside_dto.ToGenericDto(), floor_dto.ToGenericDto(), door_dto.ToGenericDto(), portal_dto.ToGenericDto(), inside_dto.ToGenericDto() });
    return outside_dto;
}

void SceneGraphMaker::SaveLazyRegion(const std::string& node_name, const std::vector<Enigma::Engine::GenericDto>& dtos)
{
    std::string json = Enigma::Gateways::DtoJsonGateway::Serialize(dtos);
    IFilePtr iFile = FileSystem::Instance()->OpenFile(Filename(node_name + ".node@DataPath"), "w+b");
    if (FATAL_LOG_EXPR(!iFile)) return;
    iFile->Write(0, convert_to_buffer(json));
    FileSystem::Instance()->CloseFile(iFile);
}

Enigma::SceneGraph::PortalDto SceneGraphMaker::MakeDirectingInsidePortal(const Enigma::MathLib::Matrix4& parent_world)
{
    PortalDto portal_dto;
    portal_dto.TheFactoryDesc() = FactoryDesc(Portal::TYPE_RTTI.GetName());
    portal_dto.Name() = "inside_portal";
    portal_dto.LocalTransform() = Matrix4::IDENTITY;
    portal_dto.WorldTransform() = parent_world * portal_dto.LocalTransform();
    portal_dto.ModelBound() = PrimitiveMeshMaker::GetDoorBound().SerializeDto().ToGenericDto();
    portal_dto.WorldBound() = BoundingVolume::CreateFromTransform(PrimitiveMeshMaker::GetDoorBound(), portal_dto.WorldTransform()).SerializeDto().ToGenericDto();
    portal_dto.SpatialFlag() = static_cast<unsigned>(Spatial::SpatialBit::Spatial_BelongToParent | Spatial::Spatial_Unlit);
    portal_dto.AdjacentZoneNodeName() = "inside_region";
    portal_dto.IsOpen() = true;
    return portal_dto;
}

Enigma::SceneGraph::PortalZoneNodeDto SceneGraphMaker::MakeInsideRegion(const std::string name, const Enigma::MathLib::Matrix4& parent_world)
{
    BoundingVolume unit_bv(Box3::UNIT_BOX);
    Matrix4 local_transform = Matrix4::IDENTITY;
    Matrix4 world_transform = parent_world * local_transform;
    PawnDto board_dto = MakeBoardPawn(world_transform);

    PortalZoneNodeDto inside_dto;
    inside_dto.TheFactoryDesc() = FactoryDesc(PortalZoneNode::TYPE_RTTI.GetName());
    inside_dto.Name() = name;
    inside_dto.LocalTransform() = Matrix4::IDENTITY;
    inside_dto.WorldTransform() = parent_world * inside_dto.LocalTransform();
    inside_dto.ModelBound() = PrimitiveMeshMaker::GetBoardBound().SerializeDto().ToGenericDto();
    inside_dto.WorldBound() = BoundingVolume::CreateFromTransform(PrimitiveMeshMaker::GetBoardBound(), inside_dto.WorldTransform()).SerializeDto().ToGenericDto();
    inside_dto.SpatialFlag() = static_cast<unsigned>(Spatial::SpatialBit::Spatial_BelongToParent | Spatial::Spatial_Unlit);
    inside_dto.ChildNames() = { "board" };

    inside_dto.IsTopLevel() = true;
    SaveLazyRegion(inside_dto.Name(), { inside_dto.ToGenericDto(), board_dto.ToGenericDto() });
    return inside_dto;
}
