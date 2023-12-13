#include "RenderablePrimitiveDtos.h"
#include "RenderablePrimitivePolicies.h"
#include "Geometries/GeometryData.h"
#include "GameEngine/EffectMaterialDto.h"
#include "GameEngine/EffectTextureMapDto.h"
#include "GameEngine/GenericDto.h"
#include "Animators/ModelPrimitiveAnimator.h"
#include "Animators/AnimatorDtos.h"
#include "MeshPrimitive.h"
#include "MeshNode.h"
#include "MeshNodeTree.h"
#include "ModelPrimitive.h"
#include "SkinMeshPrimitive.h"
#include "GameEngine/GenericPolicy.h"
#include "Platforms/MemoryAllocMacro.h"
#include <cassert>

using namespace Enigma::Renderer;
using namespace Enigma::Engine;
using namespace Enigma::Animators;
using namespace Enigma::Geometries;

static std::string TOKEN_NAME = "Name";
static std::string TOKEN_ID_NAME = "Id.Name";
static std::string TOKEN_ID_SEQUENCE = "Id.Sequence";
static std::string TOKEN_ID_RTTI = "Id.Rtti";
static std::string TOKEN_GEOMETRY_ID = "GeometryId";
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
static std::string TOKEN_VISUAL_TECHNIQUE_SELECTION = "VisualTechniqueSelection";

MeshPrimitiveDto::MeshPrimitiveDto() : m_factoryDesc(MeshPrimitive::TYPE_RTTI.getName()), m_renderListID(Renderer::RenderListID::Scene)
{
}

MeshPrimitiveDto MeshPrimitiveDto::fromGenericDto(const Engine::GenericDto& dto)
{
    MeshPrimitiveDto mesh;
    mesh.factoryDesc() = dto.GetRtti();
    if (const auto v = dto.TryGetValue<std::string>(TOKEN_NAME)) mesh.name() = v.value();
    if (auto n = dto.TryGetValue<std::string>(TOKEN_ID_NAME))
    {
        if (auto r = dto.TryGetValue<std::string>(TOKEN_ID_RTTI))
        {
            std::uint64_t id_seq = 0;
            if (auto s = dto.TryGetValue<std::uint64_t>(TOKEN_ID_SEQUENCE))
            {
                id_seq = s.value();
            }
            mesh.id() = PrimitiveId(n.value(), id_seq, Frameworks::Rtti::fromName(r.value()));
        }
    }
    if (const auto v = dto.TryGetValue<std::string>(TOKEN_GEOMETRY_ID)) mesh.geometryId() = v.value();
    if (const auto v = dto.TryGetValue<GenericDto>(TOKEN_RAW_GEOMETRY)) mesh.geometry() = v.value();
    if (const auto ary = dto.TryGetValue<GenericDtoCollection>(TOKEN_EFFECTS))
    {
        for (auto& eff_dto : ary.value())
        {
            mesh.effects().emplace_back(eff_dto);
        }
    }
    if (const auto ary = dto.TryGetValue<GenericDtoCollection>(TOKEN_TEXTURE_MAPS))
    {
        for (auto& tex_dto : ary.value())
        {
            mesh.textureMaps().emplace_back(tex_dto);
        }
    }
    if (const auto v = dto.TryGetValue<unsigned>(TOKEN_RENDER_LIST_ID)) mesh.renderListID() = static_cast<Renderer::RenderListID>(v.value());
    if (const auto v = dto.TryGetValue<std::string>(TOKEN_VISUAL_TECHNIQUE_SELECTION)) mesh.visualTechniqueSelection() = v.value();
    return mesh;
}

GenericDto MeshPrimitiveDto::toGenericDto() const
{
    GenericDto dto;
    dto.AddRtti(m_factoryDesc);
    dto.AddOrUpdate(TOKEN_NAME, m_name);
    dto.AddOrUpdate(TOKEN_ID_NAME, m_id.name());
    dto.AddOrUpdate(TOKEN_ID_SEQUENCE, m_id.sequence());
    dto.AddOrUpdate(TOKEN_ID_RTTI, m_id.rtti().getName());
    dto.AddOrUpdate(TOKEN_GEOMETRY_ID, m_geometryId.name());
    if (m_geometry)
    {
        dto.AddOrUpdate(TOKEN_RAW_GEOMETRY, m_geometry.value());
    }
    dto.AddOrUpdate(TOKEN_EFFECTS, m_effects);
    dto.AddOrUpdate(TOKEN_TEXTURE_MAPS, m_textureMaps);
    dto.AddOrUpdate(TOKEN_RENDER_LIST_ID, static_cast<unsigned>(m_renderListID));
    if (!m_visualTechniqueSelection.empty())
    {
        dto.AddOrUpdate(TOKEN_VISUAL_TECHNIQUE_SELECTION, m_visualTechniqueSelection);
    }
    return dto;
}

/*std::shared_ptr<GenericPolicy> MeshPrimitiveDto::meshDtoConvertToPolicy(const GenericDto& dto, const std::shared_ptr<Engine::IDtoDeserializer>& deserializer)
{
    MeshPrimitiveDto mesh_dto = MeshPrimitiveDto::fromGenericDto(dto);
    auto policy = std::make_shared<MeshPrimitivePolicy>();
    policy->Name() = mesh_dto.m_name;
    policy->TheDtoDeserializer() = deserializer;
    if (mesh_dto.m_geometry)
    {
        policy->geometryPolicy() = GeometryDataPolicy(mesh_dto.m_geometryName, mesh_dto.m_geometry.value());
        policy->geometryFactoryDesc() = mesh_dto.m_geometry.value().GetRtti();
    }
    else
    {
        policy->geometryPolicy() = GeometryDataPolicy(mesh_dto.m_geometryName, mesh_dto.m_geometryFactory.GetResourceFilename(), deserializer);
        policy->geometryFactoryDesc() = mesh_dto.m_geometryFactory;
    }
    for (auto& eff : mesh_dto.m_effects)
    {
        policy->effectDtos().emplace_back(EffectMaterialDto::fromGenericDto(eff));
    }
    for (auto& tex : mesh_dto.m_textureMaps)
    {
        policy->textureDtos().emplace_back(EffectTextureMapDto::fromGenericDto(tex));
    }
    policy->renderListId() = mesh_dto.m_renderListID;
    policy->visualTechniqueSelection() = mesh_dto.m_visualTechniqueSelection;
    return policy;
}*/

MeshPrimitiveMetaDto::MeshPrimitiveMetaDto(const MeshPrimitiveDto& dto)
{
    for (auto& eff : dto.effects())
    {
        m_effects.emplace_back(EffectMaterialDto::fromGenericDto(eff));
    }
    for (auto& tex : dto.textureMaps())
    {
        m_textureMaps.emplace_back(EffectTextureMapDto::fromGenericDto(tex));
    }
}

SkinMeshPrimitiveDto::SkinMeshPrimitiveDto() : MeshPrimitiveDto()
{
    m_factoryDesc = FactoryDesc(SkinMeshPrimitive::TYPE_RTTI.getName());
}

SkinMeshPrimitiveDto::SkinMeshPrimitiveDto(const MeshPrimitiveDto& mesh) : MeshPrimitiveDto(mesh)
{
    assert(Frameworks::Rtti::isExactlyOrDerivedFrom(m_factoryDesc.GetRttiName(), SkinMeshPrimitive::TYPE_RTTI.getName()));
}

SkinMeshPrimitiveDto SkinMeshPrimitiveDto::fromGenericDto(const Engine::GenericDto& dto)
{
    return MeshPrimitiveDto::fromGenericDto(dto);
}

GenericDto SkinMeshPrimitiveDto::toGenericDto() const
{
    GenericDto dto = MeshPrimitiveDto::toGenericDto();
    return dto;
}

std::shared_ptr<GenericPolicy> SkinMeshPrimitiveDto::skinMeshDtoConvertToPolicy(const GenericDto& dto, const std::shared_ptr<Engine::IDtoDeserializer>& deserializer)
{
    return nullptr;
    //auto mesh_policy = std::dynamic_pointer_cast<MeshPrimitivePolicy, GenericPolicy>(MeshPrimitiveDto::meshDtoConvertToPolicy(dto, deserializer));
    //return std::make_shared<SkinMeshPrimitivePolicy>(*mesh_policy);
}

MeshNodeDto::MeshNodeDto() : m_factoryDesc(MeshNode::TYPE_RTTI.getName())
{
}

MeshNodeDto MeshNodeDto::fromGenericDto(const Engine::GenericDto& dto)
{
    MeshNodeDto node;
    node.factoryDesc() = dto.GetRtti();
    if (const auto v = dto.TryGetValue<std::string>(TOKEN_NAME)) node.name() = v.value();
    if (const auto v = dto.TryGetValue<MathLib::Matrix4>(TOKEN_LOCAL_T_POS_TRANSFORM)) node.localT_PosTransform() = v.value();
    //if (const auto v = dto.TryGetValue<MathLib::Matrix4>(TOKEN_ROOT_REF_TRANSFORM)) node.RootRefTransform() = v.value();
    if (const auto v = dto.TryGetValue<GenericDto>(TOKEN_MESH_PRIMITIVE_OBJECT)) node.meshPrimitive() = v.value();
    if (const auto v = dto.TryGetValue<unsigned>(TOKEN_PARENT_NODE_INDEX)) node.parentIndexInArray() = v.value();
    return node;
}

GenericDto MeshNodeDto::toGenericDto() const
{
    GenericDto dto;
    dto.AddRtti(m_factoryDesc);
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

MeshNodeTreeDto::MeshNodeTreeDto() : m_factoryDesc(MeshNodeTree::TYPE_RTTI.getName())
{
}

MeshNodeTreeDto MeshNodeTreeDto::fromGenericDto(const GenericDto& dto)
{
    MeshNodeTreeDto tree;
    tree.m_factoryDesc = dto.GetRtti();
    if (const auto v = dto.TryGetValue<GenericDtoCollection>(TOKEN_MESH_NODES)) tree.meshNodes() = v.value();
    return tree;
}

GenericDto MeshNodeTreeDto::toGenericDto() const
{
    GenericDto dto;
    dto.AddRtti(m_factoryDesc);
    dto.AddOrUpdate(TOKEN_MESH_NODES, m_nodeDtos);
    return dto;
}

ModelPrimitiveDto::ModelPrimitiveDto() : m_factoryDesc(ModelPrimitive::TYPE_RTTI.getName())
{
}

ModelPrimitiveDto ModelPrimitiveDto::fromGenericDto(const GenericDto& dto)
{
    ModelPrimitiveDto model;
    if (const auto v = dto.TryGetValue<std::string>(TOKEN_NAME)) model.name() = v.value();
    if (auto n = dto.TryGetValue<std::string>(TOKEN_ID_NAME))
    {
        if (auto r = dto.TryGetValue<std::string>(TOKEN_ID_RTTI))
        {
            std::uint64_t id_seq = 0;
            if (auto s = dto.TryGetValue<std::uint64_t>(TOKEN_ID_SEQUENCE))
            {
                id_seq = s.value();
            }
            model.id() = PrimitiveId(n.value(), id_seq, Frameworks::Rtti::fromName(r.value()));
        }
    }
    if (const auto v = dto.TryGetValue<GenericDto>(TOKEN_MESH_NODE_TREE)) model.nodeTree() = v.value();
    if (const auto v = dto.TryGetValue<GenericDto>(TOKEN_MODEL_ANIMATOR)) model.animator() = v.value();
    return model;
}

GenericDto ModelPrimitiveDto::toGenericDto() const
{
    GenericDto dto;
    dto.AddRtti(m_factoryDesc);
    dto.AddOrUpdate(TOKEN_NAME, m_name);
    dto.AddOrUpdate(TOKEN_ID_NAME, m_id.name());
    dto.AddOrUpdate(TOKEN_ID_SEQUENCE, m_id.sequence());
    dto.AddOrUpdate(TOKEN_ID_RTTI, m_id.rtti().getName());
    dto.AddOrUpdate(TOKEN_MESH_NODE_TREE, m_nodeTreeDto);
    if (m_animatorDto) dto.AddOrUpdate(TOKEN_MODEL_ANIMATOR, m_animatorDto.value());
    return dto;
}

std::shared_ptr<GenericPolicy> ModelPrimitiveDto::modelDtoConvertToPolicy(const GenericDto& dto, const std::shared_ptr<Engine::IDtoDeserializer>& deserializer)
{
    ModelPrimitiveDto model_dto = ModelPrimitiveDto::fromGenericDto(dto);
    auto policy = std::make_shared<ModelPrimitivePolicy>();
    policy->Name() = model_dto.m_name;
    policy->TheDtoDeserializer() = deserializer;
    policy->nodeTreeDto() = MeshNodeTreeDto::fromGenericDto(model_dto.m_nodeTreeDto);
    if ((model_dto.m_animatorDto) && (model_dto.m_animatorDto->GetRtti().GetRttiName() == ModelPrimitiveAnimator::TYPE_RTTI.getName()))
    {
        policy->modelAnimator() = ModelAnimatorDto::fromGenericDto(model_dto.m_animatorDto.value());
    }
    return policy;
}
