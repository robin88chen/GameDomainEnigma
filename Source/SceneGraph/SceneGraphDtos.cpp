#include "SceneGraphDtos.h"
#include "LazyNode.h"
#include "Node.h"
#include "GameEngine/BoundingVolumeDto.h"
#include "Spatial.h"
#include "Light.h"
#include "Pawn.h"
#include "SceneGraphPolicies.h"
#include "VisibilityManagedNode.h"
#include "GameEngine/DtoDeserializer.h"
#include <cassert>

using namespace Enigma::SceneGraph;
using namespace Enigma::MathLib;
using namespace Enigma::Engine;

static std::string TOKEN_NAME = "Name";
static std::string TOKEN_PARENT_NAME = "ParentName";
static std::string TOKEN_LOCAL_TRANSFORM = "LocalTransform";
static std::string TOKEN_WORLD_TRANSFORM = "WorldTransform";
static std::string TOKEN_GRAPH_DEPTH = "GraphDepth";
static std::string TOKEN_WORLD_BOUND = "WorldBound";
static std::string TOKEN_MODEL_BOUND = "ModelBound";
static std::string TOKEN_CULLING_MODE = "CullingMode";
static std::string TOKEN_SPATIAL_FLAG = "SpatialFlag";
static std::string TOKEN_NOTIFY_FLAG = "NotifyFlag";
static std::string TOKEN_CHILD_NAMES = "ChildNames";
static std::string TOKEN_LIGHT_INFO = "LightInfo";
static std::string TOKEN_PAWN_PRIMITIVE = "PawnPrimitive";
//static std::string TOKEN_PRIMITIVE_FACTORY = "PrimitiveFactory";

SpatialDto::SpatialDto() : m_factoryDesc(Spatial::TYPE_RTTI.getName()), m_isTopLevel(false), m_graphDepth(0), m_cullingMode(0), m_spatialFlag(0), m_notifyFlag(0)
{
    m_localTransform = Matrix4::IDENTITY;
    m_worldTransform = Matrix4::IDENTITY;
    BoundingVolume bv{ Box3::UNIT_BOX };
    m_modelBound = bv.serializeDto().toGenericDto();
    m_worldBound = bv.serializeDto().toGenericDto();
}

SpatialDto SpatialDto::fromGenericDto(const GenericDto& dto)
{
    SpatialDto spatial_dto;
    spatial_dto.factoryDesc() = dto.GetRtti();
    spatial_dto.m_isTopLevel = dto.IsTopLevel();
    if (auto v = dto.TryGetValue<std::string>(TOKEN_NAME)) spatial_dto.name() = v.value();
    if (auto v = dto.TryGetValue<std::string>(TOKEN_PARENT_NAME)) spatial_dto.parentName() = v.value();
    if (auto v = dto.TryGetValue<Matrix4>(TOKEN_LOCAL_TRANSFORM)) spatial_dto.localTransform() = v.value();
    if (auto v = dto.TryGetValue<Matrix4>(TOKEN_WORLD_TRANSFORM)) spatial_dto.worldTransform() = v.value();
    if (auto v = dto.TryGetValue<unsigned int>(TOKEN_GRAPH_DEPTH)) spatial_dto.graphDepth() = v.value();
    if (auto v = dto.TryGetValue<GenericDto>(TOKEN_MODEL_BOUND)) spatial_dto.modelBound() = v.value();
    if (auto v = dto.TryGetValue<GenericDto>(TOKEN_WORLD_BOUND)) spatial_dto.worldBound() = v.value();
    if (auto v = dto.TryGetValue<unsigned int>(TOKEN_CULLING_MODE)) spatial_dto.cullingMode() = v.value();
    if (auto v = dto.TryGetValue<unsigned int>(TOKEN_SPATIAL_FLAG)) spatial_dto.spatialFlag() = v.value();
    if (auto v = dto.TryGetValue<unsigned int>(TOKEN_NOTIFY_FLAG)) spatial_dto.notifyFlag() = v.value();
    return spatial_dto;
}

GenericDto SpatialDto::toGenericDto() const
{
    GenericDto dto;
    dto.AddRtti(m_factoryDesc);
    dto.AsTopLevel(m_isTopLevel);
    dto.AddOrUpdate(TOKEN_NAME, m_name);
    if (!m_parentName.empty()) dto.AddOrUpdate(TOKEN_PARENT_NAME, m_parentName);
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

NodeDto::NodeDto() : SpatialDto()
{
    m_factoryDesc = FactoryDesc(Node::TYPE_RTTI.getName());
}

NodeDto::NodeDto(const SpatialDto& spatial_dto) : SpatialDto(spatial_dto)
{
    assert(Frameworks::Rtti::isExactlyOrDerivedFrom(m_factoryDesc.GetRttiName(), Node::TYPE_RTTI.getName()));
}

NodeDto NodeDto::fromGenericDto(const GenericDto& dto)
{
    NodeDto node_dto(SpatialDto::fromGenericDto(dto));
    if (auto v = dto.TryGetValue<std::vector<std::string>>(TOKEN_CHILD_NAMES)) node_dto.m_childNames = v.value();
    return node_dto;
}

GenericDto NodeDto::toGenericDto() const
{
    GenericDto dto = SpatialDto::toGenericDto();
    dto.AddOrUpdate(TOKEN_CHILD_NAMES, m_childNames);

    return dto;
}

LightDto::LightDto() : SpatialDto()
{
    m_factoryDesc = FactoryDesc(Light::TYPE_RTTI.getName());
}

LightDto::LightDto(const SpatialDto& spatial_dto) : SpatialDto(spatial_dto)
{
    assert(Frameworks::Rtti::isExactlyOrDerivedFrom(m_factoryDesc.GetRttiName(), Light::TYPE_RTTI.getName()));
}

LightDto LightDto::fromGenericDto(const Engine::GenericDto& dto)
{
    LightDto light_dto(SpatialDto::fromGenericDto(dto));
    if (auto v = dto.TryGetValue<GenericDto>(TOKEN_LIGHT_INFO)) light_dto.m_lightInfo = v.value();
    return light_dto;
}

GenericDto LightDto::toGenericDto() const
{
    GenericDto dto = SpatialDto::toGenericDto();
    dto.AddOrUpdate(TOKEN_LIGHT_INFO, m_lightInfo);

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

PawnDto PawnDto::fromGenericDto(const Engine::GenericDto& dto)
{
    PawnDto pawn_dto(SpatialDto::fromGenericDto(dto));
    if (auto v = dto.TryGetValue<GenericDto>(TOKEN_PAWN_PRIMITIVE)) pawn_dto.m_primitive = v.value();
    //if (auto v = dto.TryGetValue<FactoryDesc>(TOKEN_PRIMITIVE_FACTORY)) pawn_dto.m_primitiveFactory = v.value();
    return pawn_dto;
}

GenericDto PawnDto::toGenericDto() const
{
    GenericDto dto = SpatialDto::toGenericDto();
    if (m_primitive) dto.AddOrUpdate(TOKEN_PAWN_PRIMITIVE, m_primitive.value());
    //dto.AddOrUpdate(TOKEN_PRIMITIVE_FACTORY, m_primitiveFactory);
    return dto;
}

std::shared_ptr<PawnPolicy> PawnDto::convertToPolicy(const std::shared_ptr<Engine::IDtoDeserializer>& deserializer)
{
    if (m_primitive)
    {
        return std::make_shared<PawnPolicy>(m_name, m_primitive.value());
    }
    //todo : prefab support
    /*else
    {
        return std::make_shared<PawnPolicy>(m_name, m_primitiveFactory.GetPrefab(), deserializer);
    }*/
    return nullptr;
}

LazyNodeDto::LazyNodeDto() : NodeDto()
{
    m_factoryDesc = FactoryDesc(LazyNode::TYPE_RTTI.getName());
}

LazyNodeDto::LazyNodeDto(const NodeDto& node_dto) : NodeDto(node_dto)
{
    assert(Frameworks::Rtti::isExactlyOrDerivedFrom(m_factoryDesc.GetRttiName(), LazyNode::TYPE_RTTI.getName()));
}

LazyNodeDto LazyNodeDto::fromGenericDto(const Engine::GenericDto& dto)
{
    LazyNodeDto node_dto(NodeDto::fromGenericDto(dto));
    return node_dto;
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

VisibilityManagedNodeDto VisibilityManagedNodeDto::fromGenericDto(const Engine::GenericDto& dto)
{
    VisibilityManagedNodeDto node_dto(LazyNodeDto::fromGenericDto(dto));
    return node_dto;
}

GenericDto VisibilityManagedNodeDto::toGenericDto() const
{
    GenericDto dto = LazyNodeDto::toGenericDto();
    return dto;
}
