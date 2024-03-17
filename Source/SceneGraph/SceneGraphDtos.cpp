#include "SceneGraphDtos.h"
#include "LazyNode.h"
#include "Node.h"
#include "GameEngine/BoundingVolumeDto.h"
#include "Spatial.h"
#include "Light.h"
#include "Pawn.h"
#include "VisibilityManagedNode.h"
#include <cassert>

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
static std::string TOKEN_CHILDREN = "Children";
static std::string TOKEN_NATIVE_DTO = "NativeDto";
static std::string TOKEN_LIGHT_INFO = "LightInfo";
static std::string TOKEN_PAWN_PRIMITIVE_ID = "PawnPrimitiveId";

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

NodeDto::ChildDto::ChildDto(const Engine::GenericDto& dto_from)
{
    if (auto v = dto_from.tryGetValue<std::vector<std::string>>(TOKEN_ID)) m_id = v.value();
    if (auto v = dto_from.tryGetValue<GenericDto>(TOKEN_NATIVE_DTO)) m_dto = v.value();
}

NodeDto::ChildDto::ChildDto(const SpatialId& id) : m_id(id)
{
}

NodeDto::ChildDto::ChildDto(const SpatialId& id, const Engine::GenericDto& child_dto) : m_id(id), m_dto(child_dto)
{
}

GenericDto NodeDto::ChildDto::toGenericDto() const
{
    GenericDto dto;
    dto.addOrUpdate(TOKEN_ID, m_id.tokens());
    if (m_dto) dto.addOrUpdate(TOKEN_NATIVE_DTO, m_dto.value());
    return dto;
}

NodeDto::NodeDto() : SpatialDto()
{
    m_factoryDesc = FactoryDesc(Node::TYPE_RTTI.getName());
}

NodeDto::NodeDto(const SpatialDto& spatial_dto) : SpatialDto(spatial_dto)
{
    assert(Frameworks::Rtti::isExactlyOrDerivedFrom(m_factoryDesc.GetRttiName(), Node::TYPE_RTTI.getName()));
}

NodeDto::NodeDto(const Engine::GenericDto& dto) : SpatialDto(dto)
{
    assert(Frameworks::Rtti::isExactlyOrDerivedFrom(m_factoryDesc.GetRttiName(), Node::TYPE_RTTI.getName()));
    if (auto children = dto.tryGetValue<Engine::GenericDtoCollection>(TOKEN_CHILDREN))
    {
        for (const Engine::GenericDto& child : children.value())
        {
            m_children.emplace_back(child);
        }
    }
}

GenericDto NodeDto::toGenericDto() const
{
    GenericDto dto = SpatialDto::toGenericDto();
    Engine::GenericDtoCollection dtos;
    for (const ChildDto& child : m_children)
    {
        dtos.push_back(child.toGenericDto());
    }
    dto.addOrUpdate(TOKEN_CHILDREN, dtos);
    return dto;
}

std::vector<SpatialId> NodeDto::tokensToIds(const std::vector<std::string>& child_tokens)
{
    std::vector<SpatialId> child_ids;
    for (unsigned i = 0; i < child_tokens.size(); i += 2)
    {
        child_ids.push_back(SpatialId({ child_tokens[i], child_tokens[i + 1] }));
    };
    return child_ids;
}

std::vector<std::string> NodeDto::idsToTokens(const std::vector<SpatialId> child_ids) const
{
    std::vector<std::string> child_tokens;
    for (const SpatialId& id : child_ids)
    {
        auto tokens = id.tokens();
        child_tokens.insert(child_tokens.end(), tokens.begin(), tokens.end());
    }
    return child_tokens;
}

LightDto::LightDto() : SpatialDto()
{
    m_factoryDesc = FactoryDesc(Light::TYPE_RTTI.getName());
}

LightDto::LightDto(const SpatialDto& spatial_dto) : SpatialDto(spatial_dto)
{
    assert(Frameworks::Rtti::isExactlyOrDerivedFrom(m_factoryDesc.GetRttiName(), Light::TYPE_RTTI.getName()));
}

LightDto::LightDto(const Engine::GenericDto& dto) : SpatialDto(dto)
{
    assert(Frameworks::Rtti::isExactlyOrDerivedFrom(m_factoryDesc.GetRttiName(), Light::TYPE_RTTI.getName()));
    if (auto v = dto.tryGetValue<GenericDto>(TOKEN_LIGHT_INFO)) m_lightInfo = v.value();
}

GenericDto LightDto::toGenericDto() const
{
    GenericDto dto = SpatialDto::toGenericDto();
    dto.addOrUpdate(TOKEN_LIGHT_INFO, m_lightInfo);

    return dto;
}

PawnDto::PawnDto() : SpatialDto()
{
    m_factoryDesc = FactoryDesc(Pawn::TYPE_RTTI.getName());
}

PawnDto::PawnDto(const SpatialDto& spatial_dto) : SpatialDto(spatial_dto)
{
    assert(Frameworks::Rtti::isExactlyOrDerivedFrom(m_factoryDesc.GetRttiName(), Pawn::TYPE_RTTI.getName()));
}

PawnDto::PawnDto(const Engine::GenericDto& dto) : SpatialDto(dto)
{
    assert(Frameworks::Rtti::isExactlyOrDerivedFrom(m_factoryDesc.GetRttiName(), Pawn::TYPE_RTTI.getName()));
    if (auto v = dto.tryGetValue<std::vector<std::string>>(TOKEN_PAWN_PRIMITIVE_ID)) m_primitiveId = v.value();
}

GenericDto PawnDto::toGenericDto() const
{
    GenericDto dto = SpatialDto::toGenericDto();
    if (m_primitiveId)
    {
        dto.addOrUpdate(TOKEN_PAWN_PRIMITIVE_ID, m_primitiveId.value().tokens());
    }
    return dto;
}

LazyNodeDto::LazyNodeDto() : NodeDto()
{
    m_factoryDesc = FactoryDesc(LazyNode::TYPE_RTTI.getName());
}

LazyNodeDto::LazyNodeDto(const NodeDto& node_dto) : NodeDto(node_dto)
{
    assert(Frameworks::Rtti::isExactlyOrDerivedFrom(m_factoryDesc.GetRttiName(), LazyNode::TYPE_RTTI.getName()));
}

LazyNodeDto::LazyNodeDto(const Engine::GenericDto& dto) : NodeDto(dto)
{
}

GenericDto LazyNodeDto::toGenericDto() const
{
    GenericDto dto = NodeDto::toGenericDto();
    return dto;
}

VisibilityManagedNodeDto::VisibilityManagedNodeDto() : LazyNodeDto()
{
    m_factoryDesc = FactoryDesc(VisibilityManagedNode::TYPE_RTTI.getName());
}

VisibilityManagedNodeDto::VisibilityManagedNodeDto(const LazyNodeDto& lazy_node_dto) : LazyNodeDto(lazy_node_dto)
{
    assert(Frameworks::Rtti::isExactlyOrDerivedFrom(m_factoryDesc.GetRttiName(), VisibilityManagedNode::TYPE_RTTI.getName()));
}

VisibilityManagedNodeDto::VisibilityManagedNodeDto(const Engine::GenericDto& dto) : LazyNodeDto(dto)
{
}

GenericDto VisibilityManagedNodeDto::toGenericDto() const
{
    GenericDto dto = LazyNodeDto::toGenericDto();
    return dto;
}
