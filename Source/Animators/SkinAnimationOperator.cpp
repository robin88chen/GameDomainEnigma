#include "SkinAnimationOperator.h"
#include "Platforms/PlatformLayer.h"
#include "Renderer/SkinMeshPrimitive.h"

using namespace Enigma::Renderer;
using namespace Enigma::Animators;
using namespace Enigma::MathLib;

DEFINE_RTTI_OF_BASE(Animators, SkinAnimationOperator);

SkinAnimationOperator::SkinAnimationOperator()
{
}

SkinAnimationOperator::SkinAnimationOperator(const SkinAnimationOperator& op)
{
    if (!op.m_skinMeshPrim.expired()) m_skinMeshPrim = op.m_skinMeshPrim.lock();
    m_boneNodeNames = op.m_boneNodeNames;
    m_nodeOffsets = op.m_nodeOffsets;
    m_skinNodeIndexMapping = op.m_skinNodeIndexMapping;
}

SkinAnimationOperator::SkinAnimationOperator(SkinAnimationOperator&& op)
{
    if (!op.m_skinMeshPrim.expired()) m_skinMeshPrim = std::move(op.m_skinMeshPrim.lock());
    m_boneNodeNames = std::move(op.m_boneNodeNames);
    m_nodeOffsets = std::move(op.m_nodeOffsets);
    m_skinNodeIndexMapping = std::move(op.m_skinNodeIndexMapping);
}

SkinAnimationOperator::~SkinAnimationOperator()
{
    if (!m_skinMeshPrim.expired())
    {
        m_skinMeshPrim.lock()->LoosePrimitiveBoneMatrix();
    }
}

SkinAnimationOperator& SkinAnimationOperator::operator=(const SkinAnimationOperator& op)
{
    if (!op.m_skinMeshPrim.expired()) m_skinMeshPrim = op.m_skinMeshPrim.lock();
    m_boneNodeNames = op.m_boneNodeNames;
    m_nodeOffsets = op.m_nodeOffsets;
    m_skinNodeIndexMapping = op.m_skinNodeIndexMapping;
    return *this;
}

SkinAnimationOperator& SkinAnimationOperator::operator=(SkinAnimationOperator&& op)
{
    if (!op.m_skinMeshPrim.expired()) m_skinMeshPrim = std::move(op.m_skinMeshPrim.lock());
    m_boneNodeNames = std::move(op.m_boneNodeNames);
    m_nodeOffsets = std::move(op.m_nodeOffsets);
    m_skinNodeIndexMapping = std::move(op.m_skinNodeIndexMapping);
    return *this;
}

SkinOperatorDto SkinAnimationOperator::SerializeDto()
{
    SkinOperatorDto dto;
    if (!m_skinMeshPrim.expired()) dto.SkinMeshName() = m_skinMeshPrim.lock()->GetName();
    dto.BoneNodeNames() = m_boneNodeNames;
    dto.NodeOffsets() = m_nodeOffsets;
    return dto;
}

void SkinAnimationOperator::UpdateSkinMeshBoneMatrix(const MeshNodeTree& mesh_node_tree)
{
    if (FATAL_LOG_EXPR(m_skinMeshPrim.expired())) return;
    if (FATAL_LOG_EXPR(!m_skinNodeIndexMapping.size())) return;
    // mesh prim 的頂點都是相對於 mesh node, but, skin mesh 的 bone, offset 計算都以 root ref 為基礎
    // 是以要將所有bone matrix 都再乘上 inv. ref., 這樣所有變形後的頂點,均是相對於 mesh node
    Matrix4 mxInvRef = m_skinMeshPrim.lock()->GetOwnerRootRefTransform().Inverse();

    for (unsigned i = 0; i < m_skinNodeIndexMapping.size(); i++)
    {
        auto node_index = m_skinNodeIndexMapping[i];
        if (!node_index) continue;
        auto mesh_node = mesh_node_tree.GetMeshNode(node_index.value());
        if (!mesh_node) continue;
        Matrix4 mx = mxInvRef * mesh_node.value().get().GetRootRefTransform() * m_nodeOffsets[i];
        m_skinMeshPrim.lock()->UpdateBoneEffectMatrix(i, mx);
    }
}

void SkinAnimationOperator::CalculateNodeOffsetMatrix(const std::shared_ptr<ModelPrimitive>& model, const Matrix4& root_ref_trans)
{
    if (FATAL_LOG_EXPR(!model)) return;
    if (FATAL_LOG_EXPR(!m_boneNodeNames.size())) return;
    const unsigned bone_count = static_cast<unsigned>(m_boneNodeNames.size());
    m_nodeOffsets.resize(bone_count);
    m_skinNodeIndexMapping.resize(bone_count);
    for (unsigned int i = 0; i < bone_count; i++)
    {
        auto node_idx = model->GetMeshNodeTree().FindMeshNodeIndex(m_boneNodeNames[i]);
        m_skinNodeIndexMapping[i] = node_idx;
        if (!node_idx) continue;
        auto mesh_node = model->GetMeshNodeTree().GetMeshNode(node_idx.value());
        if (!mesh_node) continue;
        m_nodeOffsets[i] = mesh_node.value().get().GetRootRefTransform().Inverse() * root_ref_trans;
    }
    if (!m_skinMeshPrim.expired()) m_skinMeshPrim.lock()->CreateBoneMatrixArray(bone_count);
}

void SkinAnimationOperator::LinkSkinMeshPrimitive(const std::shared_ptr<SkinMeshPrimitive>& prim,
    const std::vector<std::string>& boneNodeNames)
{
    m_skinMeshPrim = prim;
    m_boneNodeNames = boneNodeNames;
    if (!m_skinMeshPrim.expired())
    {
        m_skinMeshPrim.lock()->BindPrimitiveBoneMatrix();
    }
}

void SkinAnimationOperator::LinkNodeOffsetMatrix(const std::shared_ptr<ModelPrimitive>& model,
    const std::vector<Matrix4>& boneNodeOffsets)
{
    // 前提是，bone name & bone offset 順序是相同的
    if (FATAL_LOG_EXPR(!model)) return;
    if (FATAL_LOG_EXPR(!m_boneNodeNames.size())) return;
    const unsigned bone_count = static_cast<unsigned>(m_boneNodeNames.size());
    m_nodeOffsets.resize(bone_count);
    m_skinNodeIndexMapping.resize(bone_count);
    for (unsigned int i = 0; i < bone_count; i++)
    {
        auto node_idx = model->GetMeshNodeTree().FindMeshNodeIndex(m_boneNodeNames[i]);
        m_skinNodeIndexMapping[i] = node_idx;
        if (!node_idx) continue;
        m_nodeOffsets[i] = boneNodeOffsets[i];
    }
    if (!m_skinMeshPrim.expired()) m_skinMeshPrim.lock()->CreateBoneMatrixArray(bone_count);
}

void SkinAnimationOperator::ReLinkClonedSkinMesh(const std::shared_ptr<SkinMeshPrimitive>& prim)
{
    assert(prim);
    m_skinMeshPrim = prim;
    if (!m_skinMeshPrim.expired())
    {
        m_skinMeshPrim.lock()->BindPrimitiveBoneMatrix();
    }
}
