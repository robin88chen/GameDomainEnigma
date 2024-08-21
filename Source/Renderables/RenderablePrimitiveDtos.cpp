#include "RenderablePrimitiveDtos.h"
#include "Geometries/GeometryData.h"
#include "GameEngine/GenericDto.h"
#include "MeshPrimitive.h"
#include "MeshNode.h"
#include "MeshNodeTree.h"
#include "ModelPrimitive.h"
#include "SkinMeshPrimitive.h"
#include <cassert>

using namespace Enigma::Renderables;
using namespace Enigma::Engine;
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

/*MeshPrimitiveDto::MeshPrimitiveDto() : m_factoryDesc(MeshPrimitive::TYPE_RTTI.getName()), m_renderListID(Renderer::Renderer::RenderListID::Scene)
{
}

MeshPrimitiveDto::MeshPrimitiveDto(const Engine::GenericDto& dto) : m_factoryDesc(dto.getRtti()), m_renderListID(Renderer::Renderer::RenderListID::Scene)
{
    factoryDesc() = dto.getRtti();
    if (auto v = dto.tryGetValue<std::vector<std::string>>(TOKEN_ID)) id() = v.value();
    if (const auto v = dto.tryGetValue<std::string>(TOKEN_GEOMETRY_ID)) geometryId() = v.value();
    if (const auto v = dto.tryGetValue<GenericDto>(TOKEN_RAW_GEOMETRY)) geometry() = v.value();
    if (const auto ary = dto.tryGetValue<std::vector<std::string>>(TOKEN_EFFECTS))
    {
        for (auto& eff_id : ary.value())
        {
            effects().emplace_back(eff_id);
        }
    }
    if (const auto ary = dto.tryGetValue<GenericDtoCollection>(TOKEN_TEXTURE_MAPS))
    {
        for (auto& tex_dto : ary.value())
        {
            textureMaps().emplace_back(tex_dto);
        }
    }
    if (const auto v = dto.tryGetValue<unsigned>(TOKEN_RENDER_LIST_ID)) renderListID() = static_cast<Renderer::Renderer::RenderListID>(v.value());
    if (const auto v = dto.tryGetValue<std::string>(TOKEN_VISUAL_TECHNIQUE_SELECTION)) visualTechniqueSelection() = v.value();
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
}*/

/*MeshPrimitiveMetaDto::MeshPrimitiveMetaDto(const MeshPrimitiveDto& dto)
{
    for (auto& eff : dto.effects())
    {
        m_effects.emplace_back(eff);
    }
    for (auto& tex : dto.textureMaps())
    {
        EffectTextureMapDisassembler disassembler;
        disassembler.disassemble(tex);
        m_textureMaps.emplace_back(disassembler);
    }
}*/

void MeshPrimitiveMetaDto::replaceDuplicatedEffects(const std::shared_ptr<MeshPrimitive>& mesh_primitive)
{
    if (!mesh_primitive) return;
    m_effects.clear();
    for (unsigned i = 0; i < mesh_primitive->getEffectMaterialCount(); ++i)
    {
        m_effects.emplace_back(mesh_primitive->getEffectMaterial(i)->id());
    }
}

SkinMeshPrimitiveDto::SkinMeshPrimitiveDto() //: MeshPrimitiveDto()
{
    //m_factoryDesc = FactoryDesc(SkinMeshPrimitive::TYPE_RTTI.getName());
}

SkinMeshPrimitiveDto::SkinMeshPrimitiveDto(const Engine::GenericDto& dto) //: MeshPrimitiveDto(dto)
{
}

/*SkinMeshPrimitiveDto::SkinMeshPrimitiveDto(const MeshPrimitiveDto& mesh) : MeshPrimitiveDto(mesh)
{
    assert(Frameworks::Rtti::isExactlyOrDerivedFrom(m_factoryDesc.GetRttiName(), SkinMeshPrimitive::TYPE_RTTI.getName()));
}*/

GenericDto SkinMeshPrimitiveDto::toGenericDto() const
{
    GenericDto dto; // = MeshPrimitiveDto::toGenericDto();
    return dto;
}

MeshNodeDto::MeshNodeDto() : m_factoryDesc(MeshNode::TYPE_RTTI.getName())
{
}

MeshNodeDto::MeshNodeDto(const Engine::GenericDto& dto) : m_factoryDesc(dto.getRtti())
{
    factoryDesc() = dto.getRtti();
    if (const auto v = dto.tryGetValue<std::string>(TOKEN_NAME)) name() = v.value();
    if (const auto v = dto.tryGetValue<MathLib::Matrix4>(TOKEN_LOCAL_T_POS_TRANSFORM)) localT_PosTransform() = v.value();
    //if (const auto v = dto.tryGetValue<MathLib::Matrix4>(TOKEN_ROOT_REF_TRANSFORM)) RootRefTransform() = v.value();
    if (auto v = dto.tryGetValue<std::vector<std::string>>(TOKEN_MESH_ID)) meshPrimitiveId() = v.value();
    if (const auto v = dto.tryGetValue<unsigned>(TOKEN_PARENT_NODE_INDEX)) parentIndexInArray() = v.value();
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

MeshNodeTreeDto::MeshNodeTreeDto(const Engine::GenericDto& dto) : m_factoryDesc(dto.getRtti())
{
    m_factoryDesc = dto.getRtti();
    if (const auto v = dto.tryGetValue<GenericDtoCollection>(TOKEN_MESH_NODES)) meshNodes() = v.value();
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

ModelPrimitiveDto::ModelPrimitiveDto(const Engine::GenericDto& dto) : m_factoryDesc(dto.getRtti())
{
    factoryDesc() = dto.getRtti();
    if (auto v = dto.tryGetValue<std::vector<std::string>>(TOKEN_ID)) id() = v.value();
    if (const auto v = dto.tryGetValue<GenericDto>(TOKEN_MESH_NODE_TREE)) nodeTree() = v.value();
    if (const auto v = dto.tryGetValue<std::vector<std::string>>(TOKEN_MODEL_ANIMATOR_ID)) animatorId() = v.value();
}

GenericDto ModelPrimitiveDto::toGenericDto() const
{
    GenericDto dto;
    dto.addRtti(m_factoryDesc);
    dto.addOrUpdate(TOKEN_ID, m_id.tokens());
    dto.addOrUpdate(TOKEN_MESH_NODE_TREE, m_nodeTreeDto);
    if (m_animatorId) dto.addOrUpdate(TOKEN_MODEL_ANIMATOR_ID, m_animatorId.value().tokens());
    return dto;
}
