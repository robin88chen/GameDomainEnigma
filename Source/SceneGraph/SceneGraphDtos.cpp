#include "SceneGraphDtos.h"
#include "Node.h"
#include "GameEngine/BoundingVolumeDto.h"
#include "Spatial.h"
#include "Light.h"

using namespace Enigma::SceneGraph;
using namespace Enigma::MathLib;
using namespace Enigma::Engine;

static std::string TOKEN_NAME = "Name";
static std::string TOKEN_LOCAL_TRANSFORM = "LocalTransform";
static std::string TOKEN_WORLD_TRANSFORM = "LocalTransform";
static std::string TOKEN_GRAPH_DEPTH = "GraphDepth";
static std::string TOKEN_WORLD_BOUND = "WorldBound";
static std::string TOKEN_MODEL_BOUND = "ModelBound";
static std::string TOKEN_CULLING_MODE = "CullingMode";
static std::string TOKEN_SPATIAL_FLAG = "SpatialFlag";
static std::string TOKEN_NOTIFY_FLAG = "NotifyFlag";
static std::string TOKEN_CHILD_NAMES = "ChildNames";
static std::string TOKEN_LIGHT_INFO = "LightInfo";

SpatialDto SpatialDto::FromGenericDto(const GenericDto& dto)
{
    SpatialDto spatial_dto;
    spatial_dto.m_isTopLevel = dto.IsTopLevel();
    spatial_dto.m_name = dto.TryGetValue<std::string>(TOKEN_NAME).value();
    spatial_dto.m_localTransform = dto.TryGetValue<Matrix4>(TOKEN_LOCAL_TRANSFORM).value();
    spatial_dto.m_worldTransform = dto.TryGetValue<Matrix4>(TOKEN_WORLD_TRANSFORM).value();
    spatial_dto.m_graphDepth = dto.TryGetValue<unsigned int>(TOKEN_GRAPH_DEPTH).value();
    spatial_dto.m_modelBound = dto.TryGetValue<GenericDto>(TOKEN_MODEL_BOUND).value();
    spatial_dto.m_worldBound = dto.TryGetValue<GenericDto>(TOKEN_WORLD_BOUND).value();
    spatial_dto.m_cullingMode = dto.TryGetValue<unsigned int>(TOKEN_CULLING_MODE).value();
    spatial_dto.m_spatialFlag = dto.TryGetValue<unsigned int>(TOKEN_SPATIAL_FLAG).value();
    spatial_dto.m_notifyFlag = dto.TryGetValue<unsigned int>(TOKEN_NOTIFY_FLAG).value();
    return spatial_dto;
}

GenericDto SpatialDto::ToGenericDto()
{
    GenericDto dto;
    dto.AsTopLevel(m_isTopLevel);
    dto.AddRtti(FactoryDesc(Spatial::TYPE_RTTI.GetName()));
    dto.AddOrUpdate(TOKEN_NAME, m_name);
    dto.AddOrUpdate(TOKEN_LOCAL_TRANSFORM, m_localTransform);
    dto.AddOrUpdate(TOKEN_WORLD_TRANSFORM, m_worldTransform);
    dto.AddOrUpdate(TOKEN_GRAPH_DEPTH, m_graphDepth);
    dto.AddOrUpdate(TOKEN_MODEL_BOUND, m_modelBound);
    dto.AddOrUpdate(TOKEN_WORLD_BOUND, m_worldBound);
    dto.AddOrUpdate(TOKEN_CULLING_MODE, m_cullingMode);
    dto.AddOrUpdate(TOKEN_SPATIAL_FLAG, m_spatialFlag);
    dto.AddOrUpdate(TOKEN_NOTIFY_FLAG, m_notifyFlag);
    return dto;
}

NodeDto::NodeDto(const SpatialDto& spatial_dto) : SpatialDto(spatial_dto)
{
}

NodeDto NodeDto::FromGenericDto(const GenericDto& dto)
{
    NodeDto node_dto(SpatialDto::FromGenericDto(dto));
    node_dto.m_childNames = dto.TryGetValue<std::vector<std::string>>(TOKEN_CHILD_NAMES).value();
    return node_dto;
}

GenericDto NodeDto::ToGenericDto()
{
    GenericDto dto = SpatialDto::ToGenericDto();
    dto.AddRtti(FactoryDesc(Node::TYPE_RTTI.GetName()));
    dto.AddOrUpdate(TOKEN_CHILD_NAMES, m_childNames);

    return dto;
}

LightDto::LightDto(const SpatialDto& spatial_dto) : SpatialDto(spatial_dto)
{
}

LightDto LightDto::FromGenericDto(const Engine::GenericDto& dto)
{
    LightDto light_dto(SpatialDto::FromGenericDto(dto));
    light_dto.m_lightInfo = dto.TryGetValue<GenericDto>(TOKEN_LIGHT_INFO).value();
    return light_dto;
}

GenericDto LightDto::ToGenericDto()
{
    GenericDto dto = SpatialDto::ToGenericDto();
    dto.AddRtti(FactoryDesc(Light::TYPE_RTTI.GetName()));
    dto.AddOrUpdate(TOKEN_LIGHT_INFO, m_lightInfo);

    return dto;
}
