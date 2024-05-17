/*****************************************************************
 * \file   QuadTreeRoot.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   April 2024
 ******************************************************************/
#ifndef QUAD_TREE_ROOT_H
#define QUAD_TREE_ROOT_H

#include "QuadTreeRootId.h"
#include "GameEngine/GenericDto.h"
#include "SceneGraph/SpatialId.h"
#include "SceneGraph/LazyNode.h"
#include "GameEngine/BoundingVolume.h"
#include "SceneGraph/SceneGraphRepository.h"

namespace Enigma::WorldMap
{
    class QuadTreeVolume;
    class QuadTreeRoot
    {
        DECLARE_EN_RTTI_NON_BASE;
    public:
        QuadTreeRoot(const QuadTreeRootId& id);
        QuadTreeRoot(const QuadTreeRootId& id, const Engine::GenericDto& o);

        const Engine::FactoryDesc& factoryDesc() const { return m_factoryDesc; }
        void factoryDesc(const Engine::FactoryDesc& fd) { m_factoryDesc = fd; }

        Engine::GenericDto serializeDto() const;

        const QuadTreeRootId& id() const { return m_id; }
        const SceneGraph::SpatialId& rootNodeId() const { return m_rootNodeId; }

        static std::shared_ptr<QuadTreeRoot> queryQuadTreeRoot(const QuadTreeRootId& id);

        std::optional<SceneGraph::SpatialId> findFittingNode(const Engine::BoundingVolume& bv_in_world) const;
        //! design note: Since multiple nodes need to be created and hydration needs to be done, so directly reference the repository and no need to modify the hydration request or command.
        //! design note: If the creation of fitting nodes go to more complex, it will move to other classes to handle some day.
        void createFittingNodes(const std::shared_ptr<SceneGraph::SceneGraphRepository>& repository, const Engine::BoundingVolume& bv_in_world, unsigned max_depth);

    protected:
        std::error_code createTreeNode(const std::shared_ptr<SceneGraph::SceneGraphRepository>& repository, const std::shared_ptr<QuadTreeVolume>& volume);
        Engine::GenericDto assembleChildTreeNode(const SceneGraph::SpatialId& parent_id, const Engine::FactoryDesc& parent_desc, const SceneGraph::SpatialId& id, const MathLib::Matrix4& local_transform, const MathLib::Matrix4& world_transform, const Engine::BoundingVolume& model_bound);
        std::string replaceToChildFilename(const std::string& parent_filename, const std::string& path_id, const SceneGraph::SpatialId& parent_id, const SceneGraph::SpatialId& id);

    protected:
        Engine::FactoryDesc m_factoryDesc;
        QuadTreeRootId m_id;
        SceneGraph::SpatialId m_rootNodeId;
        std::weak_ptr<SceneGraph::LazyNode> m_root;
    };
}

#endif // QUAD_TREE_ROOT_H
