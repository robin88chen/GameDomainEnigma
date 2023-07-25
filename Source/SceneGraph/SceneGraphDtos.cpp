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
static std::string TOKEN_PRIMITIVE_FACTORY = "PrimitiveFactory";

SpatialDto::SpatialDto() : m_factoryDesc(Spatial::TYPE_RTTI.GetName()), m_isTopLevel(false), m_graphDepth(0), m_cullingMode(0), m_spatialFlag(0), m_notifyFlag(0)
{
    m_localTransform = Matrix4::IDENTITY;
    m_worldTransform = Matrix4::IDENTITY;
    BoundingVolume bv{ Box3::UNIT_BOX };
    m_modelBound = bv.SerializeDto().ToGenericDto();
    m_worldBound = bv.SerializeDto().ToGenericDto();
}

SpatialDto SpatialDto::FromGenericDto(const GenericDto& dto)
{
    SpatialDto spatial_dto;
    spatial_dto.TheFactoryDesc() = dto.GetRtti();
    spatial_dto.m_isTopLevel = dto.IsTopLevel();
    if (auto v = dto.TryGetValue<std::string>(TOKEN_NAME)) spatial_dto.Name() = v.value();
    if (auto v = dto.TryGetValue<Matrix4>(TOKEN_LOCAL_TRANSFORM)) spatial_dto.LocalTransform() = v.value();
    if (auto v = dto.TryGetValue<Matrix4>(TOKEN_WORLD_TRANSFORM)) spatial_dto.WorldTransform() = v.value();
    if (auto v = dto.TryGetValue<unsigned int>(TOKEN_GRAPH_DEPTH)) spatial_dto.GraphDepth() = v.value();
    if (auto v = dto.TryGetValue<GenericDto>(TOKEN_MODEL_BOUND)) spatial_dto.ModelBound() = v.value();
    if (auto v = dto.TryGetValue<GenericDto>(TOKEN_WORLD_BOUND)) spatial_dto.WorldBound() = v.value();
    if (auto v = dto.TryGetValue<unsigned int>(TOKEN_CULLING_MODE)) spatial_dto.CullingMode() = v.value();
    if (auto v = dto.TryGetValue<unsigned int>(TOKEN_SPATIAL_FLAG)) spatial_dto.SpatialFlag() = v.value();
    if (auto v = dto.TryGetValue<unsigned int>(TOKEN_NOTIFY_FLAG)) spatial_dto.NotifyFlag() = v.value();
    return spatial_dto;
}

GenericDto SpatialDto::ToGenericDto() const
{
    GenericDto dto;
    dto.AddRtti(m_factoryDesc);
    dto.AsTopLevel(m_isTopLevel);
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

NodeDto::NodeDto() : SpatialDto()
{
    m_factoryDesc = FactoryDesc(Node::TYPE_RTTI.GetName());
}

NodeDto::NodeDto(const SpatialDto& spatial_dto) : SpatialDto(spatial_dto)
{
    assert(Frameworks::Rtti::IsExactlyOrDerivedFrom(m_factoryDesc.GetRttiName(), Node::TYPE_RTTI.GetName()));
}

NodeDto NodeDto::FromGenericDto(const GenericDto& dto)
{
    NodeDto node_dto(SpatialDto::FromGenericDto(dto));
    if (auto v = dto.TryGetValue<std::vector<std::string>>(TOKEN_CHILD_NAMES)) node_dto.m_childNames = v.value();
    return node_dto;
}

GenericDto NodeDto::ToGenericDto() const
{
    GenericDto dto = SpatialDto::ToGenericDto();
    dto.AddOrUpdate(TOKEN_CHILD_NAMES, m_childNames);

    return dto;
}

LightDto::LightDto() : SpatialDto()
{
    m_factoryDesc = FactoryDesc(Light::TYPE_RTTI.GetName());
}

LightDto::LightDto(const SpatialDto& spatial_dto) : SpatialDto(spatial_dto)
{
    assert(Frameworks::Rtti::IsExactlyOrDerivedFrom(m_factoryDesc.GetRttiName(), Light::TYPE_RTTI.GetName()));
}

LightDto LightDto::FromGenericDto(const Engine::GenericDto& dto)
{
    LightDto light_dto(SpatialDto::FromGenericDto(dto));
    if (auto v = dto.TryGetValue<GenericDto>(TOKEN_LIGHT_INFO)) light_dto.m_lightInfo = v.value();
    return light_dto;
}

GenericDto LightDto::ToGenericDto() const
{
    GenericDto dto = SpatialDto::ToGenericDto();
    dto.AddOrUpdate(TOKEN_LIGHT_INFO, m_lightInfo);

    return dto;
}

PawnDto::PawnDto() : SpatialDto(), m_primitiveFactory(Primitive::TYPE_RTTI.GetName())
{
    m_factoryDesc = FactoryDesc(Pawn::TYPE_RTTI.GetName());
}

PawnDto::PawnDto(const SpatialDto& spatial_dto) : SpatialDto(spatial_dto), m_primitiveFactory(Primitive::TYPE_RTTI.GetName())
{
    assert(Frameworks::Rtti::IsExactlyOrDerivedFrom(m_factoryDesc.GetRttiName(), Pawn::TYPE_RTTI.GetName()));
}

PawnDto PawnDto::FromGenericDto(const Engine::GenericDto& dto)
{
    PawnDto pawn_dto(SpatialDto::FromGenericDto(dto));
    if (auto v = dto.TryGetValue<GenericDto>(TOKEN_PAWN_PRIMITIVE)) pawn_dto.m_primitive = v.value();
    if (auto v = dto.TryGetValue<FactoryDesc>(TOKEN_PRIMITIVE_FACTORY)) pawn_dto.m_primitiveFactory = v.value();
    return pawn_dto;
}

GenericDto PawnDto::ToGenericDto() const
{
    GenericDto dto = SpatialDto::ToGenericDto();
    if (m_primitive) dto.AddOrUpdate(TOKEN_PAWN_PRIMITIVE, m_primitive.value());
    dto.AddOrUpdate(TOKEN_PRIMITIVE_FACTORY, m_primitiveFactory);
    return dto;
}

std::shared_ptr<PawnPolicy> PawnDto::ConvertToPolicy(const std::shared_ptr<Engine::IDtoDeserializer>& deserializer)
{
    if (m_primitive)
    {
        return std::make_shared<PawnPolicy>(m_name, m_primitive.value());
    }
    else
    {
        return std::make_shared<PawnPolicy>(m_name, m_primitiveFactory.GetPrefab(), deserializer);
    }
}

LazyNodeDto::LazyNodeDto() : NodeDto()
{
    m_factoryDesc = FactoryDesc(LazyNode::TYPE_RTTI.GetName());
}

LazyNodeDto::LazyNodeDto(const NodeDto& node_dto) : NodeDto(node_dto)
{
    assert(Frameworks::Rtti::IsExactlyOrDerivedFrom(m_factoryDesc.GetRttiName(), LazyNode::TYPE_RTTI.GetName()));
}

LazyNodeDto LazyNodeDto::FromGenericDto(const Engine::GenericDto& dto)
{
    LazyNodeDto node_dto(NodeDto::FromGenericDto(dto));
    return node_dto;
}

GenericDto LazyNodeDto::ToGenericDto() const
{
    GenericDto dto = NodeDto::ToGenericDto();
    return dto;
}

VisibilityManagedNodeDto::VisibilityManagedNodeDto() : LazyNodeDto()
{
    m_factoryDesc = FactoryDesc(VisibilityManagedNode::TYPE_RTTI.GetName());
}

VisibilityManagedNodeDto::VisibilityManagedNodeDto(const LazyNodeDto& lazy_node_dto) : LazyNodeDto(lazy_node_dto)
{
    assert(Frameworks::Rtti::IsExactlyOrDerivedFrom(m_factoryDesc.GetRttiName(), VisibilityManagedNode::TYPE_RTTI.GetName()));
}

VisibilityManagedNodeDto VisibilityManagedNodeDto::FromGenericDto(const Engine::GenericDto& dto)
{
    VisibilityManagedNodeDto node_dto(LazyNodeDto::FromGenericDto(dto));
    return node_dto;
}

GenericDto VisibilityManagedNodeDto::ToGenericDto() const
{
    GenericDto dto = LazyNodeDto::ToGenericDto();
    return dto;
}
