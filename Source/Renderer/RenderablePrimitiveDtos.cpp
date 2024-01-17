#include "RenderablePrimitiveDtos.h"
#include "RenderablePrimitivePolicies.h"
#include "Geometries/GeometryData.h"
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
#include <cassert>

using namespace Enigma::Renderer;
using namespace Enigma::Engine;
using namespace Enigma::Animators;
using namespace Enigma::Geometries;

static std::string TOKEN_NAME = "Name";
static std::string TOKEN_ID = "Id";
static std::string TOKEN_GEOMETRY_ID = "GeometryId";
static std::string TOKEN_RAW_GEOMETRY = "RawGeometry";
static std::string TOKEN_EFFECTS = "Effects";
static std::string TOKEN_TEXTURE_MAPS = "TextureMaps";
static std::string TOKEN_RENDER_LIST_ID = "RenderListId";
static std::string TOKEN_LOCAL_T_POS_TRANSFORM = "LocalT_PosTransform";
static std::string TOKEN_MESH_ID = "MeshId";
static std::string TOKEN_PARENT_NODE_INDEX = "ParentNodeIndex";
static std::string TOKEN_MESH_NODES = "MeshNodes";
static std::string TOKEN_MESH_NODE_TREE = "MeshNodeTree";
static std::string TOKEN_MODEL_ANIMATOR_ID = "ModelAnimatorId";
static std::string TOKEN_VISUAL_TECHNIQUE_SELECTION = "VisualTechniqueSelection";

MeshPrimitiveDto::MeshPrimitiveDto() : m_factoryDesc(MeshPrimitive::TYPE_RTTI.getName()), m_renderListID(Renderer::RenderListID::Scene)
{
}

MeshPrimitiveDto MeshPrimitiveDto::fromGenericDto(const Engine::GenericDto& dto)
{
    MeshPrimitiveDto mesh;
    mesh.factoryDesc() = dto.getRtti();
    if (auto v = dto.tryGetValue<std::vector<std::string>>(TOKEN_ID)) mesh.id() = v.value();
    if (const auto v = dto.tryGetValue<std::string>(TOKEN_GEOMETRY_ID)) mesh.geometryId() = v.value();
    if (const auto v = dto.tryGetValue<GenericDto>(TOKEN_RAW_GEOMETRY)) mesh.geometry() = v.value();
    if (const auto ary = dto.tryGetValue<std::vector<std::string>>(TOKEN_EFFECTS))
    {
        for (auto& eff_id : ary.value())
        {
            mesh.effects().emplace_back(eff_id);
        }
    }
    if (const auto ary = dto.tryGetValue<GenericDtoCollection>(TOKEN_TEXTURE_MAPS))
    {
        for (auto& tex_dto : ary.value())
        {
            mesh.textureMaps().emplace_back(tex_dto);
        }
    }
    if (const auto v = dto.tryGetValue<unsigned>(TOKEN_RENDER_LIST_ID)) mesh.renderListID() = static_cast<Renderer::RenderListID>(v.value());
    if (const auto v = dto.tryGetValue<std::string>(TOKEN_VISUAL_TECHNIQUE_SELECTION)) mesh.visualTechniqueSelection() = v.value();
    return mesh;
}

GenericDto MeshPrimitiveDto::toGenericDto() const
{
    GenericDto dto;
    dto.addRtti(m_factoryDesc);
    dto.addOrUpdate(TOKEN_ID, m_id.tokens());
    dto.addOrUpdate(TOKEN_GEOMETRY_ID, m_geometryId.name());
    if (m_geometry)
    {
        dto.addOrUpdate(TOKEN_RAW_GEOMETRY, m_geometry.value());
    }
    if (!m_effects.empty())
    {
        std::vector<std::string> effect_names;
        effect_names.reserve(m_effects.size());
        for (auto& eff : m_effects)
        {
            effect_names.emplace_back(eff.name());
        }
        dto.addOrUpdate(TOKEN_EFFECTS, effect_names);
    }
    dto.addOrUpdate(TOKEN_TEXTURE_MAPS, m_textureMaps);
    dto.addOrUpdate(TOKEN_RENDER_LIST_ID, static_cast<unsigned>(m_renderListID));
    if (!m_visualTechniqueSelection.empty())
    {
        dto.addOrUpdate(TOKEN_VISUAL_TECHNIQUE_SELECTION, m_visualTechniqueSelection);
    }
    return dto;
}

/*std::shared_ptr<GenericPolicy> MeshPrimitiveDto::meshDtoConvertToPolicy(const GenericDto& dto, const std::shared_ptr<Engine::IDtoDeserializer>& deserializer)
{
    MeshPrimitiveDto mesh_dto = MeshPrimitiveDto::fromGenericDto(dto);
    auto policy = std::make_shared<MeshPrimitivePolicy>();
    policy->name() = mesh_dto.m_name;
    policy->TheDtoDeserializer() = deserializer;
    if (mesh_dto.m_geometry)
    {
        policy->geometryPolicy() = GeometryDataPolicy(mesh_dto.m_geometryName, mesh_dto.m_geometry.value());
        policy->geometryFactoryDesc() = mesh_dto.m_geometry.value().getRtti();
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
        m_effects.emplace_back(eff);
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
    node.factoryDesc() = dto.getRtti();
    if (const auto v = dto.tryGetValue<std::string>(TOKEN_NAME)) node.name() = v.value();
    if (const auto v = dto.tryGetValue<MathLib::Matrix4>(TOKEN_LOCAL_T_POS_TRANSFORM)) node.localT_PosTransform() = v.value();
    //if (const auto v = dto.tryGetValue<MathLib::Matrix4>(TOKEN_ROOT_REF_TRANSFORM)) node.RootRefTransform() = v.value();
    if (auto v = dto.tryGetValue<std::vector<std::string>>(TOKEN_MESH_ID)) node.meshPrimitiveId() = v.value();
    if (const auto v = dto.tryGetValue<unsigned>(TOKEN_PARENT_NODE_INDEX)) node.parentIndexInArray() = v.value();
    return node;
}

GenericDto MeshNodeDto::toGenericDto() const
{
    GenericDto dto;
    dto.addRtti(m_factoryDesc);
    dto.addOrUpdate(TOKEN_NAME, m_name);
    dto.addOrUpdate(TOKEN_LOCAL_T_POS_TRANSFORM, m_localT_PosTransform);
    //dto.addOrUpdate(TOKEN_ROOT_REF_TRANSFORM, m_rootRefTransform);
    if (m_meshPrimitiveId) dto.addOrUpdate(TOKEN_MESH_ID, m_meshPrimitiveId.value().tokens());
    if (m_parentIndexInArray)
    {
        dto.addOrUpdate(TOKEN_PARENT_NODE_INDEX, m_parentIndexInArray.value());
    }
    return dto;
}

MeshNodeTreeDto::MeshNodeTreeDto() : m_factoryDesc(MeshNodeTree::TYPE_RTTI.getName())
{
}

MeshNodeTreeDto MeshNodeTreeDto::fromGenericDto(const GenericDto& dto)
{
    MeshNodeTreeDto tree;
    tree.m_factoryDesc = dto.getRtti();
    if (const auto v = dto.tryGetValue<GenericDtoCollection>(TOKEN_MESH_NODES)) tree.meshNodes() = v.value();
    return tree;
}

GenericDto MeshNodeTreeDto::toGenericDto() const
{
    GenericDto dto;
    dto.addRtti(m_factoryDesc);
    dto.addOrUpdate(TOKEN_MESH_NODES, m_nodeDtos);
    return dto;
}

ModelPrimitiveDto::ModelPrimitiveDto() : m_factoryDesc(ModelPrimitive::TYPE_RTTI.getName())
{
}

ModelPrimitiveDto ModelPrimitiveDto::fromGenericDto(const GenericDto& dto)
{
    ModelPrimitiveDto model;
    model.factoryDesc() = dto.getRtti();
    if (auto v = dto.tryGetValue<std::vector<std::string>>(TOKEN_ID)) model.id() = v.value();
    if (const auto v = dto.tryGetValue<GenericDto>(TOKEN_MESH_NODE_TREE)) model.nodeTree() = v.value();
    if (const auto v = dto.tryGetValue<std::vector<std::string>>(TOKEN_MODEL_ANIMATOR_ID)) model.animatorId() = v.value();
    return model;
}

GenericDto ModelPrimitiveDto::toGenericDto() const
{
    GenericDto dto;
    dto.addRtti(m_factoryDesc);
    dto.addOrUpdate(TOKEN_ID, m_id.tokens());
    dto.addOrUpdate(TOKEN_MESH_NODE_TREE, m_nodeTreeDto);
    if (m_animatorId) dto.addOrUpdate(TOKEN_MODEL_ANIMATOR_ID, m_animatorId.value());
    return dto;
}

/*std::shared_ptr<GenericPolicy> ModelPrimitiveDto::modelDtoConvertToPolicy(const GenericDto& dto, const std::shared_ptr<Engine::IDtoDeserializer>& deserializer)
{
    ModelPrimitiveDto model_dto = ModelPrimitiveDto::fromGenericDto(dto);
    auto policy = std::make_shared<ModelPrimitivePolicy>();
    policy->TheDtoDeserializer() = deserializer;
    policy->nodeTreeDto() = MeshNodeTreeDto::fromGenericDto(model_dto.m_nodeTreeDto);
    if ((model_dto.m_animatorDto) && (model_dto.m_animatorDto->getRtti().GetRttiName() == ModelPrimitiveAnimator::TYPE_RTTI.getName()))
    {
        policy->modelAnimator() = ModelAnimatorDto::fromGenericDto(model_dto.m_animatorDto.value());
    }
    return policy;
}*/
