/*********************************************************************
 * \file   SceneQuadTreeRoot.h
 * \brief  scene graph quad tree root, an aggregate,
 *      compose root lazy node, not inherit from lazy node, then we can have different root node type
 * \author Lancelot 'Robin' Chen
 * \date   November 2023
 *********************************************************************/
#ifndef SCENE_QUAD_TREE_ROOT_H
#define SCENE_QUAD_TREE_ROOT_H

#include "Frameworks/Rtti.h"
#include "GameEngine/BoundingVolume.h"
#include "SceneGraph/LazyNode.h"
#include "SceneGraph/Node.h"
#include <string>
#include <memory>

namespace Enigma::WorldMap
{
    class SceneQuadTreeRoot
    {
        DECLARE_EN_RTTI_NON_BASE;
    public:
        SceneQuadTreeRoot(const std::string& name, const std::shared_ptr<SceneGraph::LazyNode>& root);
        ~SceneQuadTreeRoot();
        SceneQuadTreeRoot(const SceneQuadTreeRoot& other) = delete;
        SceneQuadTreeRoot(SceneQuadTreeRoot&& other) noexcept = delete;
        SceneQuadTreeRoot& operator=(const SceneQuadTreeRoot& other) = delete;
        SceneQuadTreeRoot& operator=(SceneQuadTreeRoot&& other) noexcept = delete;

        Engine::GenericDto serializeDto();
        std::vector<Engine::GenericDtoCollection> serializeTreeGraphs();

        Engine::FactoryDesc& factoryDesc() { return m_factory_desc; }
        const std::string& name() const { return m_name; }

        std::shared_ptr<SceneGraph::Node> queryFittingNode(const Engine::BoundingVolume& bv_in_world) const;

    protected:
        std::shared_ptr<SceneGraph::Node> findFittingNodeFromRoot(const Engine::BoundingVolume& bv_in_root) const;
        std::shared_ptr<SceneGraph::Node> findFittingLeaf(const Engine::BoundingVolume& bv_in_root) const;

        std::tuple<MathLib::Box3, unsigned> locateSubTreeBoxAndIndex(const MathLib::Box3& parent_box, const MathLib::Vector3& local_pos) const;
        bool testSubTreeQuadEnvelop(const MathLib::Box3& quad_box_in_parent, const Engine::BoundingVolume& bv_in_parent) const;
        std::shared_ptr<SceneGraph::Node> findTargetSubtree(const std::shared_ptr<SceneGraph::Node>& any_level_parent, const std::string& parent_name, unsigned sub_tree_index) const;

    protected:
        Engine::FactoryDesc m_factory_desc;
        std::string m_name;
        std::weak_ptr<SceneGraph::LazyNode> m_root;
    };
}

#endif // SCENE_QUAD_TREE_ROOT_H
