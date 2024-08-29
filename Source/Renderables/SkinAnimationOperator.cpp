#include "SkinAnimationOperator.h"
#include "Platforms/PlatformLayer.h"
#include "Renderables/SkinMeshPrimitive.h"
#include "SkinOperatorAssembler.h"

using namespace Enigma::Renderables;
using namespace Enigma::MathLib;

DEFINE_RTTI_OF_BASE(Renderables, SkinAnimationOperator);

SkinAnimationOperator::SkinAnimationOperator() : m_factoryDesc(TYPE_RTTI.getName())
{
}

SkinAnimationOperator::SkinAnimationOperator(const SkinAnimationOperator& op) : m_factoryDesc(op.m_factoryDesc)
{
    m_skinMeshId = op.m_skinMeshId;
    m_boneNodeNames = op.m_boneNodeNames;
    m_nodeOffsets = op.m_nodeOffsets;
    m_t_posNodeOffsets = op.m_t_posNodeOffsets;
    m_skinNodeIndexMapping = op.m_skinNodeIndexMapping;
}

SkinAnimationOperator::SkinAnimationOperator(SkinAnimationOperator&& op) noexcept : m_factoryDesc(op.m_factoryDesc)
{
    m_skinMeshId = op.m_skinMeshId;
    m_boneNodeNames = std::move(op.m_boneNodeNames);
    m_nodeOffsets = std::move(op.m_nodeOffsets);
    m_t_posNodeOffsets = std::move(op.m_t_posNodeOffsets);
    m_skinNodeIndexMapping = std::move(op.m_skinNodeIndexMapping);
}

SkinAnimationOperator::~SkinAnimationOperator()
{
}

SkinAnimationOperator& SkinAnimationOperator::operator=(const SkinAnimationOperator& op)
{
    if (this == &op) return *this;
    m_factoryDesc = op.m_factoryDesc;
    m_skinMeshId = op.m_skinMeshId;
    m_boneNodeNames = op.m_boneNodeNames;
    m_nodeOffsets = op.m_nodeOffsets;
    m_t_posNodeOffsets = op.m_t_posNodeOffsets;
    m_skinNodeIndexMapping = op.m_skinNodeIndexMapping;
    return *this;
}

SkinAnimationOperator& SkinAnimationOperator::operator=(SkinAnimationOperator&& op) noexcept
{
    m_factoryDesc = op.m_factoryDesc;
    m_skinMeshId = op.m_skinMeshId;
    m_boneNodeNames = std::move(op.m_boneNodeNames);
    m_nodeOffsets = std::move(op.m_nodeOffsets);
    m_t_posNodeOffsets = std::move(op.m_t_posNodeOffsets);
    m_skinNodeIndexMapping = std::move(op.m_skinNodeIndexMapping);
    return *this;
}

void SkinAnimationOperator::assemble(const std::shared_ptr<SkinOperatorAssembler>& assembler) const
{
    assembler->factoryDesc(m_factoryDesc);
    if (m_skinMeshId) assembler->operatedSkin(m_skinMeshId.value());
    if (!m_t_posNodeOffsets.empty())
    {
        assembler->bones(m_boneNodeNames, m_t_posNodeOffsets);
    }
    else if (!m_nodeOffsets.empty())
    {
        assembler->bones(m_boneNodeNames, m_nodeOffsets);
    }
    else
    {
        assembler->bones(m_boneNodeNames);
    }
}

void SkinAnimationOperator::disassemble(const std::shared_ptr<SkinOperatorDisassembler>& disassembler)
{
    m_factoryDesc = disassembler->factoryDesc();
    if (disassembler->operatedSkin())
    {
        m_skinMeshId = disassembler->operatedSkin().value();
        m_boneNodeNames = disassembler->bones();
    }
    if (disassembler->t_posOffsets())
    {
        m_t_posNodeOffsets = disassembler->t_posOffsets().value();
    }
}

void SkinAnimationOperator::updateSkinMeshBoneMatrix(const MeshNodeTree& mesh_node_tree)
{
    auto skin_mesh = cacheSkinMesh();
    if (FATAL_LOG_EXPR(skin_mesh == nullptr)) return;
    if (FATAL_LOG_EXPR(!m_skinNodeIndexMapping.size())) return;
    // mesh prim 的頂點都是相對於 mesh node, but, skin mesh 的 bone, offset 計算都以 root ref 為基礎
    // 是以要將所有bone matrix 都再乘上 inv. ref., 這樣所有變形後的頂點,均是相對於 mesh node
    Matrix4 mxInvRef = skin_mesh->getOwnerRootRefTransform().Inverse();

    for (unsigned i = 0; i < m_skinNodeIndexMapping.size(); i++)
    {
        auto node_index = m_skinNodeIndexMapping[i];
        if (!node_index) continue;
        auto mesh_node = mesh_node_tree.getMeshNode(node_index.value());
        if (!mesh_node) continue;
        Matrix4 mx = mxInvRef * mesh_node.value().get().getRootRefTransform() * m_nodeOffsets[i];
        skin_mesh->updateBoneEffectMatrix(i, mx);
    }
}

void SkinAnimationOperator::onAttachingMeshNodeTree(const MeshNodeTree& mesh_node_tree)
{
    const unsigned bone_count = static_cast<unsigned>(m_boneNodeNames.size());
    m_nodeOffsets.resize(bone_count);
    m_skinNodeIndexMapping.resize(bone_count);
    if (m_skinMeshId.has_value())
    {
        auto skin_id = mesh_node_tree.findInstancedPrimitiveId(m_skinMeshId.value().origin());
        if (skin_id.has_value()) m_skinMeshId = skin_id.value();
    }
    for (unsigned int i = 0; i < bone_count; i++)
    {
        auto node_idx = mesh_node_tree.findMeshNodeIndex(m_boneNodeNames[i]);
        m_skinNodeIndexMapping[i] = node_idx;
        if (!node_idx) continue;
        m_nodeOffsets[i] = t_posNodeOffset(i, mesh_node_tree.getMeshNode(node_idx.value()));
    }
    if (cacheSkinMesh()) cacheSkinMesh()->createBoneMatrixArray(bone_count);
}

void SkinAnimationOperator::onDetachingMeshNodeTree()
{
    m_skinNodeIndexMapping.clear();
    m_nodeOffsets.clear();
    if (cacheSkinMesh()) cacheSkinMesh()->clearBoneMatrixArray();
}

std::shared_ptr<SkinMeshPrimitive> SkinAnimationOperator::cacheSkinMesh()
{
    if ((!m_cachedSkinMesh.expired()) && (m_cachedSkinMesh.lock()->id() == m_skinMeshId)) return m_cachedSkinMesh.lock();
    m_cachedSkinMesh.reset();
    if (!m_skinMeshId) return nullptr;
    m_cachedSkinMesh = std::dynamic_pointer_cast<SkinMeshPrimitive>(Primitives::Primitive::queryPrimitive(m_skinMeshId.value()));
    return m_cachedSkinMesh.lock();
}

Matrix4 SkinAnimationOperator::t_posNodeOffset(unsigned index, stdext::optional_ref<const MeshNode> mesh_node)
{
    if (index < m_t_posNodeOffsets.size()) return m_t_posNodeOffsets[index];
    if ((mesh_node) && (cacheSkinMesh()))
    {
        const auto root_ref_trans = cacheSkinMesh()->getOwnerRootRefTransform();
        return mesh_node.value().get().getRootRefTransform().Inverse() * root_ref_trans;
    }
    return Matrix4::IDENTITY;
}

