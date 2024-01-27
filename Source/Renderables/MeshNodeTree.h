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
#include "Frameworks/optional_ref.hpp"
#include "GameEngine/GenericDto.h"
#include <vector>
#include <optional>

namespace Enigma::Renderables
{
    class MeshNodeTree
    {
        DECLARE_EN_RTTI_NON_BASE;
    public:
        MeshNodeTree();
        MeshNodeTree(const Engine::GenericDto& dto);
        MeshNodeTree(const MeshNodeTree& tree);
        MeshNodeTree(MeshNodeTree&& tree) noexcept;
        ~MeshNodeTree();
        MeshNodeTree& operator=(const MeshNodeTree& tree);
        MeshNodeTree& operator=(MeshNodeTree&& tree) noexcept;

        const Engine::FactoryDesc& factoryDesc() const { return m_factoryDesc; }
        Engine::FactoryDesc& factoryDesc() { return m_factoryDesc; }

        Engine::GenericDto serializeDto() const;

        std::optional<unsigned> findMeshNodeIndex(const std::string& node_name) const;
        /** add mesh node to tree
            @return  the index in array of this node */
        unsigned addMeshNode(const MeshNode& node);
        unsigned getMeshNodeCount() const { return static_cast<unsigned>(m_meshNodes.size()); };
        stdext::optional_ref<MeshNode> getMeshNode(unsigned index);
        stdext::optional_ref<const MeshNode> getMeshNode(unsigned index) const;
        std::shared_ptr<MeshPrimitive> getMeshPrimitiveInNode(unsigned index) const;
        /// is child node in sub-tree? (find parent mesh node from child mesh node)
        bool isInSubTree(unsigned child_node_index, const std::string& parent_node_name);

        void updateMeshNodeLocalTransform(const MathLib::Matrix4& mxModelRootWorld, unsigned index, const MathLib::Matrix4& mxLocal);

    protected:
        Engine::FactoryDesc m_factoryDesc;
        std::vector<MeshNode> m_meshNodes;
    };
}

#endif // _MESH_NODE_TREE_H
