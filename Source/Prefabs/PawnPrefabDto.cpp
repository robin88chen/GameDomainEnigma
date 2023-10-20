#include "PawnPrefabDto.h"
#include "SceneGraph\Pawn.h"
#include "GameEngine\FactoryDesc.h"

using namespace Enigma::Prefabs;
using namespace Enigma::SceneGraph;
using namespace Enigma::Engine;
using namespace Enigma::MathLib;

static std::string TOKEN_NAME = "Name";
static std::string TOKEN_LOCAL_TRANSFORM = "LocalTransform";
static std::string TOKEN_WORLD_TRANSFORM = "WorldTransform";
static std::string TOKEN_GRAPH_DEPTH = "GraphDepth";
static std::string TOKEN_WORLD_BOUND = "WorldBound";
static std::string TOKEN_CULLING_MODE = "CullingMode";
static std::string TOKEN_SPATIAL_FLAG = "SpatialFlag";
static std::string TOKEN_NOTIFY_FLAG = "NotifyFlag";
static std::string TOKEN_PARENT_NAME = "ParentName";

PawnPrefabDto::PawnPrefabDto() : m_factoryDesc(FactoryDesc(Pawn::TYPE_RTTI.GetName())), m_isTopLevel(false), m_graphDepth(0), m_cullingMode(0), m_spatialFlag(0), m_notifyFlag(0)
{
    m_localTransform = Matrix4::IDENTITY;
    m_worldTransform = Matrix4::IDENTITY;
    BoundingVolume bv{ Box3::UNIT_BOX };
    m_worldBound = bv.SerializeDto().ToGenericDto();
}

PawnPrefabDto PawnPrefabDto::FromGenericDto(const Engine::GenericDto& dto)
{
    PawnPrefabDto prefab_dto;
    prefab_dto.TheFactoryDesc() = dto.GetRtti();
    prefab_dto.m_isTopLevel = dto.IsTopLevel();
    if (auto v = dto.TryGetValue<std::string>(TOKEN_NAME)) prefab_dto.Name() = v.value();
    if (auto v = dto.TryGetValue<Matrix4>(TOKEN_LOCAL_TRANSFORM)) prefab_dto.LocalTransform() = v.value();
    if (auto v = dto.TryGetValue<Matrix4>(TOKEN_WORLD_TRANSFORM)) prefab_dto.WorldTransform() = v.value();
    if (auto v = dto.TryGetValue<unsigned int>(TOKEN_GRAPH_DEPTH)) prefab_dto.GraphDepth() = v.value();
    if (auto v = dto.TryGetValue<GenericDto>(TOKEN_WORLD_BOUND)) prefab_dto.WorldBound() = v.value();
    if (auto v = dto.TryGetValue<unsigned int>(TOKEN_CULLING_MODE)) prefab_dto.CullingMode() = v.value();
    if (auto v = dto.TryGetValue<unsigned int>(TOKEN_SPATIAL_FLAG)) prefab_dto.SpatialFlag() = v.value();
    if (auto v = dto.TryGetValue<unsigned int>(TOKEN_NOTIFY_FLAG)) prefab_dto.NotifyFlag() = v.value();
    if (auto v = dto.TryGetValue<std::string>(TOKEN_PARENT_NAME)) prefab_dto.ParentName() = v.value();
    return prefab_dto;
}

Enigma::Engine::GenericDto PawnPrefabDto::ToGenericDto() const
{
    GenericDto dto;
    dto.AddRtti(m_factoryDesc);
    dto.AsTopLevel(m_isTopLevel);
    dto.AddOrUpdate(TOKEN_NAME, m_name);
    dto.AddOrUpdate(TOKEN_LOCAL_TRANSFORM, m_localTransform);
    dto.AddOrUpdate(TOKEN_WORLD_TRANSFORM, m_worldTransform);
    dto.AddOrUpdate(TOKEN_GRAPH_DEPTH, m_graphDepth);
    dto.AddOrUpdate(TOKEN_WORLD_BOUND, m_worldBound);
    dto.AddOrUpdate(TOKEN_CULLING_MODE, m_cullingMode);
    dto.AddOrUpdate(TOKEN_SPATIAL_FLAG, m_spatialFlag);
    dto.AddOrUpdate(TOKEN_NOTIFY_FLAG, m_notifyFlag);
    dto.AddOrUpdate(TOKEN_PARENT_NAME, m_parentName);
    return dto;
}
