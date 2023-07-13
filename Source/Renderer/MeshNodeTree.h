/*********************************************************************
 * \file   MeshNodeTree.h
 * \brief  Mesh Node Tree, value object, used by value
 * Model Primitive 中的 Mesh Node 樹狀結構,
 * 實際上是用一個陣列存放 Mesh Node
 * \author Lancelot 'Robin' Chen
 * \date   December 2022
 *********************************************************************/
#ifndef _MESH_NODE_TREE_H
#define _MESH_NODE_TREE_H

#include "MeshNode.h"
#include "MathLib/Matrix4.h"
#include <vector>
#include <optional>

namespace Enigma::Renderer
{
    class MeshNodeTree
    {
        DECLARE_EN_RTTI_NON_BASE;
    public:
        MeshNodeTree();
        MeshNodeTree(const MeshNodeTree& tree);
        MeshNodeTree(MeshNodeTree&& tree) noexcept;
        ~MeshNodeTree();
        MeshNodeTree& operator=(const MeshNodeTree& tree);
        MeshNodeTree& operator=(MeshNodeTree&& tree) noexcept;

        Engine::GenericDto SerializeDto() const;

        std::optional<unsigned> FindMeshNodeIndex(const std::string& node_name) const;
        /** add mesh node to tree
            @return  the index in array of this node */
        unsigned AddMeshNode(const MeshNode& node);
        unsigned GetMeshNodeCount() const { return static_cast<unsigned>(m_meshNodes.size()); };
        stdext::optional_ref<MeshNode> GetMeshNode(unsigned index);
        stdext::optional_ref<const MeshNode> GetMeshNode(unsigned index) const;
        MeshPrimitivePtr GetMeshPrimitiveInNode(unsigned index);
        /// is child node in sub-tree? (find parent mesh node from child mesh node)
        bool IsInSubTree(unsigned child_node_index, const std::string& parent_node_name);

        void UpdateMeshNodeLocalTransform(const MathLib::Matrix4& mxModelRootWorld, unsigned index, const MathLib::Matrix4& mxLocal);

    protected:
        std::vector<MeshNode> m_meshNodes;
    };
}

#endif // _MESH_NODE_TREE_H
