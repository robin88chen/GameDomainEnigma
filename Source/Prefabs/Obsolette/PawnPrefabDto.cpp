﻿#include "PawnPrefabDto.h"
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

PawnPrefabDto::PawnPrefabDto() : m_factoryDesc(FactoryDesc(Pawn::TYPE_RTTI.getName())), m_isTopLevel(false), m_graphDepth(0), m_cullingMode(0), m_spatialFlag(0), m_notifyFlag(0)
{
    m_localTransform = Matrix4::IDENTITY;
    m_worldTransform = Matrix4::IDENTITY;
    BoundingVolume bv{ Box3::UNIT_BOX };
    m_worldBound = bv.serializeDto().toGenericDto();
}

PawnPrefabDto PawnPrefabDto::fromGenericDto(const Engine::GenericDto& dto)
{
    PawnPrefabDto prefab_dto;
    prefab_dto.factoryDesc() = dto.getRtti();
    prefab_dto.m_isTopLevel = dto.isTopLevel();
    if (auto v = dto.tryGetValue<std::string>(TOKEN_NAME)) prefab_dto.name() = v.value();
    if (auto v = dto.tryGetValue<Matrix4>(TOKEN_LOCAL_TRANSFORM)) prefab_dto.localTransform() = v.value();
    if (auto v = dto.tryGetValue<Matrix4>(TOKEN_WORLD_TRANSFORM)) prefab_dto.worldTransform() = v.value();
    if (auto v = dto.tryGetValue<unsigned int>(TOKEN_GRAPH_DEPTH)) prefab_dto.graphDepth() = v.value();
    if (auto v = dto.tryGetValue<GenericDto>(TOKEN_WORLD_BOUND)) prefab_dto.worldBound() = v.value();
    if (auto v = dto.tryGetValue<unsigned int>(TOKEN_CULLING_MODE)) prefab_dto.cullingMode() = v.value();
    if (auto v = dto.tryGetValue<unsigned int>(TOKEN_SPATIAL_FLAG)) prefab_dto.spatialFlag() = v.value();
    if (auto v = dto.tryGetValue<unsigned int>(TOKEN_NOTIFY_FLAG)) prefab_dto.notifyFlag() = v.value();
    return prefab_dto;
}

Enigma::Engine::GenericDto PawnPrefabDto::toGenericDto() const
{
    GenericDto dto;
    dto.addRtti(m_factoryDesc);
    dto.asTopLevel(m_isTopLevel);
    dto.addOrUpdate(TOKEN_NAME, m_name);
    dto.addOrUpdate(TOKEN_LOCAL_TRANSFORM, m_localTransform);
    dto.addOrUpdate(TOKEN_WORLD_TRANSFORM, m_worldTransform);
    dto.addOrUpdate(TOKEN_GRAPH_DEPTH, m_graphDepth);
    dto.addOrUpdate(TOKEN_WORLD_BOUND, m_worldBound);
    dto.addOrUpdate(TOKEN_CULLING_MODE, m_cullingMode);
    dto.addOrUpdate(TOKEN_SPATIAL_FLAG, m_spatialFlag);
    dto.addOrUpdate(TOKEN_NOTIFY_FLAG, m_notifyFlag);
    return dto;
}
