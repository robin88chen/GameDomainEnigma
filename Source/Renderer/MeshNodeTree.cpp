#include "MeshNodeTree.h"
#include "RenderablePrimitiveDtos.h"
#include "MeshPrimitive.h"

using namespace Enigma::Renderer;
using namespace Enigma::Engine;

DEFINE_RTTI_OF_BASE(Renderer, MeshNodeTree);

MeshNodeTree::MeshNodeTree() : m_factoryDesc(MeshNodeTree::TYPE_RTTI.getName())
{
    m_meshNodes.clear();
}

MeshNodeTree::MeshNodeTree(const Engine::GenericDto& dto) : m_factoryDesc(MeshNodeTree::TYPE_RTTI.getName())
{
    MeshNodeTreeDto mesh_node_tree_dto = MeshNodeTreeDto::fromGenericDto(dto);
    m_factoryDesc = mesh_node_tree_dto.factoryDesc();
    for (auto& node_dto : mesh_node_tree_dto.meshNodes())
    {
        m_meshNodes.emplace_back(node_dto);
    }
}

MeshNodeTree::MeshNodeTree(const MeshNodeTree& tree) : m_factoryDesc(tree.factoryDesc())
{
    m_meshNodes = tree.m_meshNodes;
}

MeshNodeTree::MeshNodeTree(MeshNodeTree&& tree) noexcept : m_factoryDesc(std::move(tree.m_factoryDesc))
{
    m_meshNodes = std::move(tree.m_meshNodes);
}

MeshNodeTree::~MeshNodeTree()
{
    m_meshNodes.clear();
}

MeshNodeTree& MeshNodeTree::operator=(const MeshNodeTree& tree)
{
    m_factoryDesc = tree.m_factoryDesc;
    m_meshNodes = tree.m_meshNodes;
    return *this;
}

MeshNodeTree& MeshNodeTree::operator=(MeshNodeTree&& tree) noexcept
{
    m_factoryDesc = std::move(tree.m_factoryDesc);
    m_meshNodes = std::move(tree.m_meshNodes);
    return *this;
}

GenericDto MeshNodeTree::serializeDto() const
{
    MeshNodeTreeDto dto;
    dto.factoryDesc() = m_factoryDesc;
    for (auto& node : m_meshNodes)
    {
        dto.meshNodes().emplace_back(node.serializeDto());
    }
    return dto.toGenericDto();
}

std::optional<unsigned> MeshNodeTree::FindMeshNodeIndex(const std::string& node_name) const
{
    if (m_meshNodes.empty()) return std::nullopt;
    unsigned count = static_cast<unsigned>(m_meshNodes.size());
    for (unsigned i = 0; i < count; i++)
    {
        if (m_meshNodes[i].getName() == node_name) return i;
    }
    return std::nullopt;
}

unsigned MeshNodeTree::AddMeshNode(const MeshNode& node)
{
    m_meshNodes.emplace_back(node);
    const unsigned idx = static_cast<unsigned>(m_meshNodes.size() - 1);

    if (const auto parent_index = node.GetParentIndexInArray())  // has parent node
    {
        m_meshNodes[idx].SetRootRefTransform(m_meshNodes[parent_index.value()].GetRootRefTransform() * m_meshNodes[idx].getLocalTransform());
    }
    else
    {
        m_meshNodes[idx].SetRootRefTransform(m_meshNodes[idx].getLocalTransform());
    }
    return idx;
}

stdext::optional_ref<MeshNode> MeshNodeTree::GetMeshNode(unsigned index)
{
    if (index >= m_meshNodes.size()) return std::nullopt;
    return m_meshNodes[index];
}

stdext::optional_ref<const MeshNode> MeshNodeTree::GetMeshNode(unsigned index) const
{
    if (index >= m_meshNodes.size()) return std::nullopt;
    return m_meshNodes[index];
}

std::shared_ptr<MeshPrimitive> MeshNodeTree::GetMeshPrimitiveInNode(unsigned index)
{
    if (index >= m_meshNodes.size()) return nullptr;
    return m_meshNodes[index].GetMeshPrimitive();
}

bool MeshNodeTree::IsInSubTree(unsigned child_node_index, const std::string& parent_node_name)
{
    if (child_node_index >= m_meshNodes.size()) return false;
    std::optional<unsigned> curr_node_index = child_node_index;
    while (curr_node_index)
    {
        if (m_meshNodes[curr_node_index.value()].getName() == parent_node_name) return true;
        curr_node_index = m_meshNodes[curr_node_index.value()].GetParentIndexInArray();
    }
    return false;
}

void MeshNodeTree::UpdateMeshNodeLocalTransform(const MathLib::Matrix4& mxModelRootWorld, unsigned index, const MathLib::Matrix4& mxLocal)
{
    if (index >= m_meshNodes.size()) return;
    m_meshNodes[index].setLocalTransform(mxLocal);
    auto parent_index = m_meshNodes[index].GetParentIndexInArray();
    if (parent_index)  // has parent node
    {
        m_meshNodes[index].SetRootRefTransform(m_meshNodes[parent_index.value()].GetRootRefTransform() * mxLocal);
        std::shared_ptr<MeshPrimitive> mesh_prim = m_meshNodes[index].GetMeshPrimitive();
        if (mesh_prim)
        {
            mesh_prim->updateWorldTransform(mxModelRootWorld * m_meshNodes[index].GetRootRefTransform());
        }
    }
    else
    {
        m_meshNodes[index].SetRootRefTransform(mxLocal);
        std::shared_ptr<MeshPrimitive> mesh_prim = m_meshNodes[index].GetMeshPrimitive();
        if (mesh_prim)
        {
            mesh_prim->updateWorldTransform(mxModelRootWorld * m_meshNodes[index].GetRootRefTransform());
        }
    }
}
