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
#include "GameEngine/GenericPolicy.h"
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
static std::string TOKEN_RENDER_LIST_ID = "RenderListId";
static std::string TOKEN_LOCAL_T_POS_TRANSFORM = "LocalT_PosTransform";
static std::string TOKEN_MESH_PRIMITIVE_OBJECT = "MeshPrimitiveObject";
static std::string TOKEN_PARENT_NODE_INDEX = "ParentNodeIndex";
static std::string TOKEN_MESH_NODES = "MeshNodes";
static std::string TOKEN_MESH_NODE_TREE = "MeshNodeTree";
static std::string TOKEN_MODEL_ANIMATOR = "ModelAnimator";

MeshPrimitiveDto::MeshPrimitiveDto() : m_geometryFactory(GeometryData::TYPE_RTTI.GetName()), m_renderListID(Renderer::RenderListID::Scene)
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
    if (const auto v = dto.TryGetValue<unsigned>(TOKEN_RENDER_LIST_ID)) mesh.RenderListID() = static_cast<Renderer::RenderListID>(v.value());
    return mesh;
}

GenericDto MeshPrimitiveDto::ToGenericDto() const
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
    dto.AddOrUpdate(TOKEN_RENDER_LIST_ID, static_cast<unsigned>(m_renderListID));
    dto.SetPolicyConverter([=, *this](auto d) { return ConvertToPolicy(d); });  // 務必要抓 this 內容，否則不能保證 this 存在
    return dto;
}

std::shared_ptr<GenericPolicy> MeshPrimitiveDto::ConvertToPolicy(const std::shared_ptr<Engine::IDtoDeserializer>& deserializer) const
{
    auto policy = std::make_shared<MeshPrimitivePolicy>();
    policy->Name() = m_name;
    policy->TheDtoDeserializer() = deserializer;
    if (m_geometry)
    {
        policy->GeometryPolicy() = GeometryDataPolicy(m_geometryName, m_geometry.value());
        policy->GeometryFactoryDesc() = m_geometry.value().GetRtti();
    }
    else
    {
        policy->GeometryPolicy() = GeometryDataPolicy(m_geometryName, m_geometryFactory.GetResourceFilename(), deserializer);
        policy->GeometryFactoryDesc() = m_geometryFactory;
    }
    for (auto& eff : m_effects)
    {
        policy->EffectDtos().emplace_back(EffectMaterialDto::FromGenericDto(eff));
    }
    for (auto& tex : m_textureMaps)
    {
        policy->TextureDtos().emplace_back(EffectTextureMapDto::FromGenericDto(tex));
    }
    policy->RenderListId() = m_renderListID;
    return policy;
}

SkinMeshPrimitiveDto::SkinMeshPrimitiveDto(const MeshPrimitiveDto& mesh) : MeshPrimitiveDto(mesh)
{
}

SkinMeshPrimitiveDto SkinMeshPrimitiveDto::FromGenericDto(const Engine::GenericDto& dto)
{
    return MeshPrimitiveDto::FromGenericDto(dto);
}

GenericDto SkinMeshPrimitiveDto::ToGenericDto() const
{
    GenericDto dto = MeshPrimitiveDto::ToGenericDto();
    dto.AddRtti(FactoryDesc(SkinMeshPrimitive::TYPE_RTTI.GetName()));
    dto.SetPolicyConverter([=, *this](auto d) { return ConvertToPolicy(d); });
    return dto;
}

std::shared_ptr<GenericPolicy> SkinMeshPrimitiveDto::ConvertToPolicy(const std::shared_ptr<Engine::IDtoDeserializer>& deserializer) const
{
    auto mesh_policy = std::dynamic_pointer_cast<MeshPrimitivePolicy, GenericPolicy>(MeshPrimitiveDto::ConvertToPolicy(deserializer));
    return std::make_shared<SkinMeshPrimitivePolicy>(*mesh_policy);
}

MeshNodeDto MeshNodeDto::FromGenericDto(const Engine::GenericDto& dto)
{
    MeshNodeDto node;
    if (const auto v = dto.TryGetValue<std::string>(TOKEN_NAME)) node.Name() = v.value();
    if (const auto v = dto.TryGetValue<MathLib::Matrix4>(TOKEN_LOCAL_T_POS_TRANSFORM)) node.LocalT_PosTransform() = v.value();
    //if (const auto v = dto.TryGetValue<MathLib::Matrix4>(TOKEN_ROOT_REF_TRANSFORM)) node.RootRefTransform() = v.value();
    if (const auto v = dto.TryGetValue<GenericDto>(TOKEN_MESH_PRIMITIVE_OBJECT)) node.TheMeshPrimitive() = v.value();
    if (const auto v = dto.TryGetValue<unsigned>(TOKEN_PARENT_NODE_INDEX)) node.ParentIndexInArray() = v.value();
    return node;
}

GenericDto MeshNodeDto::ToGenericDto() const
{
    GenericDto dto;
    dto.AddRtti(FactoryDesc(MeshNode::TYPE_RTTI.GetName()));
    dto.AddOrUpdate(TOKEN_NAME, m_name);
    dto.AddOrUpdate(TOKEN_LOCAL_T_POS_TRANSFORM, m_localT_PosTransform);
    //dto.AddOrUpdate(TOKEN_ROOT_REF_TRANSFORM, m_rootRefTransform);
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

GenericDto MeshNodeTreeDto::ToGenericDto() const
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

GenericDto ModelPrimitiveDto::ToGenericDto() const
{
    GenericDto dto;
    dto.AddRtti(FactoryDesc(ModelPrimitive::TYPE_RTTI.GetName()));
    dto.AddOrUpdate(TOKEN_NAME, m_name);
    dto.AddOrUpdate(TOKEN_MESH_NODE_TREE, m_nodeTreeDto);
    if (m_animatorDto) dto.AddOrUpdate(TOKEN_MODEL_ANIMATOR, m_animatorDto.value());
    dto.SetPolicyConverter([=, *this](auto d) { return ConvertToPolicy(d); });
    return dto;
}

std::shared_ptr<GenericPolicy> ModelPrimitiveDto::ConvertToPolicy(const std::shared_ptr<Engine::IDtoDeserializer>& deserializer) const
{
    auto policy = std::make_shared<ModelPrimitivePolicy>();
    policy->Name() = m_name;
    policy->TheDtoDeserializer() = deserializer;
    policy->NodeTreeDto() = MeshNodeTreeDto::FromGenericDto(m_nodeTreeDto);
    if ((m_animatorDto) && (m_animatorDto->GetRtti().GetRttiName() == ModelPrimitiveAnimator::TYPE_RTTI.GetName()))
    {
        policy->TheModelAnimator() = ModelAnimatorDto::FromGenericDto(m_animatorDto.value());
    }
    return policy;
}
