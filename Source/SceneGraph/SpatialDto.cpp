#include "SpatialDto.h"
#include "Spatial.h"

using namespace Enigma::SceneGraph;
using namespace Enigma::MathLib;
using namespace Enigma::Engine;

static std::string TOKEN_ID = "Id";
static std::string TOKEN_PARENT_NAME = "ParentName";
static std::string TOKEN_LOCAL_TRANSFORM = "LocalTransform";
static std::string TOKEN_WORLD_TRANSFORM = "WorldTransform";
static std::string TOKEN_GRAPH_DEPTH = "GraphDepth";
static std::string TOKEN_WORLD_BOUND = "WorldBound";
static std::string TOKEN_MODEL_BOUND = "ModelBound";
static std::string TOKEN_CULLING_MODE = "CullingMode";
static std::string TOKEN_SPATIAL_FLAG = "SpatialFlag";
static std::string TOKEN_NOTIFY_FLAG = "NotifyFlag";
static std::string TOKEN_PARENT_ID = "ParentId";

SpatialDto::SpatialDto() : m_factoryDesc(Spatial::TYPE_RTTI.getName()), m_isTopLevel(false), m_graphDepth(0), m_cullingMode(0), m_spatialFlag(0), m_notifyFlag(0)
{
    m_localTransform = Matrix4::IDENTITY;
    m_worldTransform = Matrix4::IDENTITY;
    BoundingVolume bv{ Box3::UNIT_BOX };
    m_modelBound = bv.serializeDto().toGenericDto();
    m_worldBound = bv.serializeDto().toGenericDto();
}

SpatialDto::SpatialDto(const Engine::GenericDto& dto) : m_factoryDesc(Spatial::TYPE_RTTI.getName()), m_isTopLevel(false), m_graphDepth(0), m_cullingMode(0), m_spatialFlag(0), m_notifyFlag(0)
{
    factoryDesc() = dto.getRtti();
    m_isTopLevel = dto.isTopLevel();
    if (auto v = dto.tryGetValue<std::vector<std::string>>(TOKEN_ID)) id() = SpatialId(v.value());
    if (auto v = dto.tryGetValue<std::string>(TOKEN_PARENT_NAME)) parentName() = v.value();
    if (auto v = dto.tryGetValue<Matrix4>(TOKEN_LOCAL_TRANSFORM)) localTransform() = v.value();
    if (auto v = dto.tryGetValue<Matrix4>(TOKEN_WORLD_TRANSFORM)) worldTransform() = v.value();
    if (auto v = dto.tryGetValue<unsigned int>(TOKEN_GRAPH_DEPTH)) graphDepth() = v.value();
    if (auto v = dto.tryGetValue<GenericDto>(TOKEN_MODEL_BOUND)) modelBound() = v.value();
    if (auto v = dto.tryGetValue<GenericDto>(TOKEN_WORLD_BOUND)) worldBound() = v.value();
    if (auto v = dto.tryGetValue<unsigned int>(TOKEN_CULLING_MODE)) cullingMode() = v.value();
    if (auto v = dto.tryGetValue<unsigned int>(TOKEN_SPATIAL_FLAG)) spatialFlag() = v.value();
    if (auto v = dto.tryGetValue<unsigned int>(TOKEN_NOTIFY_FLAG)) notifyFlag() = v.value();
    if (auto v = dto.tryGetValue<std::vector<std::string>>(TOKEN_PARENT_ID)) parentId() = SpatialId(v.value());
}

GenericDto SpatialDto::toGenericDto() const
{
    GenericDto dto;
    dto.addRtti(m_factoryDesc);
    dto.asTopLevel(m_isTopLevel);
    dto.addOrUpdate(TOKEN_ID, m_id.tokens());
    if (!m_parentName.empty()) dto.addOrUpdate(TOKEN_PARENT_NAME, m_parentName);
    dto.addOrUpdate(TOKEN_LOCAL_TRANSFORM, m_localTransform);
    dto.addOrUpdate(TOKEN_WORLD_TRANSFORM, m_worldTransform);
    dto.addOrUpdate(TOKEN_GRAPH_DEPTH, m_graphDepth);
    dto.addOrUpdate(TOKEN_MODEL_BOUND, m_modelBound);
    dto.addOrUpdate(TOKEN_WORLD_BOUND, m_worldBound);
    dto.addOrUpdate(TOKEN_CULLING_MODE, m_cullingMode);
    dto.addOrUpdate(TOKEN_SPATIAL_FLAG, m_spatialFlag);
    dto.addOrUpdate(TOKEN_NOTIFY_FLAG, m_notifyFlag);
    if (m_parentId.has_value()) dto.addOrUpdate(TOKEN_PARENT_ID, m_parentId.value().tokens());
    return dto;
}
