#include "RenderablePrimitiveDtos.h"
#include "RenderablePrimitivePolicies.h"
#include "GameEngine/GeometryData.h"
#include "GameEngine/EffectMaterialDto.h"
#include "GameEngine/EffectTextureMapDto.h"
#include "Animators/ModelPrimitiveAnimator.h"
#include "Animators/AnimatorDtos.h"
#include "MeshPrimitive.h"
#include "MeshNode.h"
#include "MeshNodeTree.h"
#include "ModelPrimitive.h"
#include "SkinMeshPrimitive.h"
#include "Platforms/MemoryAllocMacro.h"

using namespace Enigma::Renderer;
using namespace Enigma::Engine;
using namespace Enigma::Animators;

static std::string TOKEN_NAME = "Name";
static std::string TOKEN_GEOMETRY_NAME = "GeometryName";
static std::string TOKEN_RAW_GEOMETRY = "RawGeometry";
static std::string TOKEN_GEOMETRY_FACTORY = "GeometryFactory";
static std::string TOKEN_EFFECTS = "Effects";
static std::string TOKEN_TEXTURE_MAPS = "TextureMaps";
static std::string TOKEN_LOCAL_TRANSFORM = "LocalTransform";
static std::string TOKEN_ROOT_REF_TRANSFORM = "RootRefTransform";
static std::string TOKEN_MESH_PRIMITIVE_OBJECT = "MeshPrimitiveObject";
static std::string TOKEN_PARENT_NODE_INDEX = "ParentNodeIndex";
static std::string TOKEN_MESH_NODES = "MeshNodes";
static std::string TOKEN_MESH_NODE_TREE = "MeshNodeTree";
static std::string TOKEN_MODEL_ANIMATOR = "ModelAnimator";

MeshPrimitiveDto::MeshPrimitiveDto() : m_geometryFactory(GeometryData::TYPE_RTTI.GetName())
{
}

MeshPrimitiveDto MeshPrimitiveDto::FromGenericDto(const Engine::GenericDto& dto)
{
    MeshPrimitiveDto mesh;
    if (const auto v = dto.TryGetValue<std::string>(TOKEN_NAME)) mesh.Name() = v.value();
    if (const auto v = dto.TryGetValue<std::string>(TOKEN_GEOMETRY_NAME)) mesh.GeometryName() = v.value();
    if (const auto v = dto.TryGetValue<GenericDto>(TOKEN_RAW_GEOMETRY)) mesh.TheGeometry() = v.value();
    if (const auto v = dto.TryGetValue<FactoryDesc>(TOKEN_GEOMETRY_FACTORY)) mesh.GeometryFactoryDesc() = v.value();
    if (const auto ary = dto.TryGetValue<std::vector<GenericDto>>(TOKEN_EFFECTS))
    {
        for (auto& eff_dto : ary.value())
        {
            mesh.Effects().emplace_back(eff_dto);
        }
    }
    if (const auto ary = dto.TryGetValue<std::vector<GenericDto>>(TOKEN_TEXTURE_MAPS))
    {
        for (auto& tex_dto : ary.value())
        {
            mesh.TextureMaps().emplace_back(tex_dto);
        }
    }
    return mesh;
}

GenericDto MeshPrimitiveDto::ToGenericDto()
{
    GenericDto dto;
    dto.AddRtti(FactoryDesc(MeshPrimitive::TYPE_RTTI.GetName()));
    dto.AddOrUpdate(TOKEN_NAME, m_name);
    dto.AddOrUpdate(TOKEN_GEOMETRY_NAME, m_geometryName);
    if (m_geometry)
    {
        dto.AddOrUpdate(TOKEN_RAW_GEOMETRY, m_geometry.value());
    }
    dto.AddOrUpdate(TOKEN_GEOMETRY_FACTORY, m_geometryFactory);
    dto.AddOrUpdate(TOKEN_EFFECTS, m_effects);
    dto.AddOrUpdate(TOKEN_TEXTURE_MAPS, m_textureMaps);
    return dto;
}

std::shared_ptr<MeshPrimitivePolicy> MeshPrimitiveDto::ConvertToPolicy(const std::shared_ptr<Engine::IDtoDeserializer>& deserializer,
    const std::shared_ptr<IEffectCompilingProfileDeserializer>& effect_deserializer)
{
    auto policy = std::make_shared<MeshPrimitivePolicy>();
    policy->Name() = m_name;
    policy->TheDtoDeserializer() = deserializer;
    policy->TheEffectDeserializer() = effect_deserializer;
    if (m_geometry)
    {
        policy->GeometryPolicy() = GeometryDataPolicy(m_geometryName, m_geometry.value());
    }
    else
    {
        policy->GeometryPolicy() = GeometryDataPolicy(m_geometryName, m_geometryFactory.GetResourceFilename(), deserializer);
    }
    for (auto& eff : m_effects)
    {
        policy->EffectPolicies().emplace_back(EffectMaterialDto::FromGenericDto(eff).ConvertToPolicy(effect_deserializer));
    }
    for (auto& tex : m_textureMaps)
    {
        policy->TexturePolicies().emplace_back(EffectTextureMapDto::FromGenericDto(tex).ConvertToPolicy());
    }
    return policy;
}

SkinMeshPrimitiveDto::SkinMeshPrimitiveDto(const MeshPrimitiveDto& mesh) : MeshPrimitiveDto(mesh)
{
}

SkinMeshPrimitiveDto SkinMeshPrimitiveDto::FromGenericDto(const Engine::GenericDto& dto)
{
    return MeshPrimitiveDto::FromGenericDto(dto);
}

GenericDto SkinMeshPrimitiveDto::ToGenericDto()
{
    GenericDto dto = MeshPrimitiveDto::ToGenericDto();
    dto.AddRtti(FactoryDesc(SkinMeshPrimitive::TYPE_RTTI.GetName()));
    return dto;
}

std::shared_ptr<SkinMeshPrimitivePolicy> SkinMeshPrimitiveDto::ConvertToPolicy(const std::shared_ptr<Engine::IDtoDeserializer>& deserializer,
    const std::shared_ptr<Engine::IEffectCompilingProfileDeserializer>& effect_deserializer)
{
    auto mesh_policy = MeshPrimitiveDto::ConvertToPolicy(deserializer, effect_deserializer);
    return std::make_shared<SkinMeshPrimitivePolicy>(*mesh_policy);
}

MeshNodeDto MeshNodeDto::FromGenericDto(const Engine::GenericDto& dto)
{
    MeshNodeDto node;
    if (const auto v = dto.TryGetValue<std::string>(TOKEN_NAME)) node.Name() = v.value();
    if (const auto v = dto.TryGetValue<MathLib::Matrix4>(TOKEN_LOCAL_TRANSFORM)) node.LocalTransform() = v.value();
    if (const auto v = dto.TryGetValue<MathLib::Matrix4>(TOKEN_ROOT_REF_TRANSFORM)) node.RootRefTransform() = v.value();
    if (const auto v = dto.TryGetValue<GenericDto>(TOKEN_MESH_PRIMITIVE_OBJECT)) node.TheMeshPrimitive() = v.value();
    if (const auto v = dto.TryGetValue<unsigned>(TOKEN_PARENT_NODE_INDEX)) node.ParentIndexInArray() = v.value();
    return node;
}

GenericDto MeshNodeDto::ToGenericDto()
{
    GenericDto dto;
    dto.AddRtti(FactoryDesc(MeshNode::TYPE_RTTI.GetName()));
    dto.AddOrUpdate(TOKEN_NAME, m_name);
    dto.AddOrUpdate(TOKEN_LOCAL_TRANSFORM, m_localTransform);
    dto.AddOrUpdate(TOKEN_ROOT_REF_TRANSFORM, m_rootRefTransform);
    if (m_meshPrimitive)
    {
        dto.AddOrUpdate(TOKEN_MESH_PRIMITIVE_OBJECT, m_meshPrimitive.value());
    }
    if (m_parentIndexInArray)
    {
        dto.AddOrUpdate(TOKEN_PARENT_NODE_INDEX, m_parentIndexInArray.value());
    }
    return dto;
}

MeshNodeTreeDto MeshNodeTreeDto::FromGenericDto(const GenericDto& dto)
{
    MeshNodeTreeDto tree;
    if (const auto v = dto.TryGetValue<std::vector<GenericDto>>(TOKEN_MESH_NODES)) tree.MeshNodes() = v.value();
    return tree;
}

GenericDto MeshNodeTreeDto::ToGenericDto()
{
    GenericDto dto;
    dto.AddRtti(FactoryDesc(MeshNodeTree::TYPE_RTTI.GetName()));
    dto.AddOrUpdate(TOKEN_MESH_NODES, m_nodeDtos);
    return dto;
}

ModelPrimitiveDto ModelPrimitiveDto::FromGenericDto(const GenericDto& dto)
{
    ModelPrimitiveDto model;
    if (const auto v = dto.TryGetValue<std::string>(TOKEN_NAME)) model.Name() = v.value();
    if (const auto v = dto.TryGetValue<GenericDto>(TOKEN_MESH_NODE_TREE)) model.TheNodeTree() = v.value();
    if (const auto v = dto.TryGetValue<GenericDto>(TOKEN_MODEL_ANIMATOR)) model.TheAnimator() = v.value();
    return model;
}

GenericDto ModelPrimitiveDto::ToGenericDto()
{
    GenericDto dto;
    dto.AddRtti(FactoryDesc(ModelPrimitive::TYPE_RTTI.GetName()));
    dto.AddOrUpdate(TOKEN_NAME, m_name);
    dto.AddOrUpdate(TOKEN_MESH_NODE_TREE, m_nodeTreeDto);
    if (m_animatorDto) dto.AddOrUpdate(TOKEN_MODEL_ANIMATOR, m_animatorDto.value());
    return dto;
}

std::shared_ptr<ModelPrimitivePolicy> ModelPrimitiveDto::ConvertToPolicy(const std::shared_ptr<Engine::IDtoDeserializer>& deserializer,
    const std::shared_ptr<Engine::IEffectCompilingProfileDeserializer>& effect_deserializer)
{
    auto policy = std::make_shared<ModelPrimitivePolicy>();
    policy->Name() = m_name;
    policy->TheDtoDeserializer() = deserializer;
    policy->TheEffectDeserializer() = effect_deserializer;
    policy->NodeTreeDto() = MeshNodeTreeDto::FromGenericDto(m_nodeTreeDto);
    if ((m_animatorDto) && (m_animatorDto->GetRtti().GetRttiName() == ModelPrimitiveAnimator::TYPE_RTTI.GetName()))
    {
        policy->TheModelAnimator() = ModelAnimatorDto::FromGenericDto(m_animatorDto.value());
    }
    return policy;
}
