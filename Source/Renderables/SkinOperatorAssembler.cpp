#include "SkinOperatorAssembler.h"
#include "SkinAnimationOperator.h"
#include <cassert>

using namespace Enigma::Renderables;

static std::string TOKEN_SKIN_MESH_ID = "SkinMeshId";
static std::string TOKEN_SKIN_MESH_NODE_NAME = "SkinMeshNodeName";
static std::string TOKEN_BONE_NODE_NAMES = "BoneNodeNames";
static std::string TOKEN_NODE_OFFSETS = "NodeOffsets";

SkinOperatorAssembler::SkinOperatorAssembler() : m_factoryDesc(SkinAnimationOperator::TYPE_RTTI.getName())
{
}

void SkinOperatorAssembler::operatedSkin(const Primitives::PrimitiveId& id)
{
    m_skinMeshId = id;
}

void Enigma::Renderables::SkinOperatorAssembler::skinNodeName(const std::string& name)
{
    m_skinMeshNodeName = name;
}

void SkinOperatorAssembler::addBone(const std::string& name, const MathLib::Matrix4& t_pos_offset)
{
    size_t bone_count = m_boneNodeNames.size();
    size_t offset_count = 0;
    if (m_t_posOffsets)
    {
        offset_count = m_t_posOffsets.value().size();
    }
    assert(bone_count == offset_count);  // must have same size
    m_boneNodeNames.emplace_back(name);
    if (m_t_posOffsets)
    {
        m_t_posOffsets.value().emplace_back(t_pos_offset);
    }
    else
    {
        m_t_posOffsets.emplace(std::vector<MathLib::Matrix4>{t_pos_offset});
    }
}

void SkinOperatorAssembler::addBone(const std::string& name)
{
    assert(!m_t_posOffsets.has_value());  // offsets must be empty
    m_boneNodeNames.emplace_back(name);
}

void SkinOperatorAssembler::bones(const std::vector<std::string>& names, const std::vector<MathLib::Matrix4>& t_pos_offsets)
{
    m_boneNodeNames = names;
    m_t_posOffsets = t_pos_offsets;
}

void SkinOperatorAssembler::bones(const std::vector<std::string>& names)
{
    m_t_posOffsets.reset();
    m_boneNodeNames = names;
}

Enigma::Engine::GenericDto SkinOperatorAssembler::assemble() const
{
    Engine::GenericDto dto;
    dto.addRtti(m_factoryDesc);
    if (m_skinMeshId) dto.addOrUpdate(TOKEN_SKIN_MESH_ID, m_skinMeshId.value().tokens());
    if (m_skinMeshNodeName) dto.addOrUpdate(TOKEN_SKIN_MESH_NODE_NAME, m_skinMeshNodeName.value());
    dto.addOrUpdate(TOKEN_BONE_NODE_NAMES, m_boneNodeNames);
    if (m_t_posOffsets) dto.addOrUpdate(TOKEN_NODE_OFFSETS, m_t_posOffsets.value());
    return dto;
}

SkinOperatorDisassembler::SkinOperatorDisassembler() : m_factoryDesc(SkinAnimationOperator::TYPE_RTTI.getName())
{
}

SkinOperatorDisassembler::SkinOperatorDisassembler(const Engine::GenericDto& dto) : m_factoryDesc(SkinAnimationOperator::TYPE_RTTI.getName())
{
    disassemble(dto);
}

void SkinOperatorDisassembler::disassemble(const Engine::GenericDto& dto)
{
    m_factoryDesc = dto.getRtti();
    if (auto v = dto.tryGetValue<std::vector<std::string>>(TOKEN_SKIN_MESH_ID)) m_skinMeshId = v.value();
    if (auto v = dto.tryGetValue<std::string>(TOKEN_SKIN_MESH_NODE_NAME)) m_skinMeshNodeName = v.value();
    if (auto v = dto.tryGetValue<std::vector<std::string>>(TOKEN_BONE_NODE_NAMES)) m_boneNodeNames = v.value();
    if (auto v = dto.tryGetValue<std::vector<MathLib::Matrix4>>(TOKEN_NODE_OFFSETS)) m_t_posOffsets = v.value();
}
