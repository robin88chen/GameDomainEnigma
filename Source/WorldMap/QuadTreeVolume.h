/*****************************************************************
 * \file   QuadTreeVolume.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   April 2024
 ******************************************************************/
#ifndef QUAD_TREE_VOLUME_H
#define QUAD_TREE_VOLUME_H

#include "SceneGraph/SpatialId.h"
#include "GameEngine/BoundingVolume.h"
#include "SceneGraph/Node.h"
#include <array>

namespace Enigma::WorldMap
{
    class QuadTreeVolume : public std::enable_shared_from_this<QuadTreeVolume>
    {
    public:
        static constexpr std::size_t maxChildren = 4;

    public:
        QuadTreeVolume(const SceneGraph::SpatialId& id);
        QuadTreeVolume(const SceneGraph::SpatialId& id, const std::optional<SceneGraph::SpatialId>& parent_id, const MathLib::Matrix4& worldTransform, const Engine::BoundingVolume& modelBounding);

        const SceneGraph::SpatialId& id() const { return m_id; }
        const std::optional<SceneGraph::SpatialId>& parentId() const { return m_parentId; }
        const std::array<std::shared_ptr<QuadTreeVolume>, maxChildren>& children() const { return m_children; }
        MathLib::Matrix4 worldTransform() const { return m_worldTransform; }
        Engine::BoundingVolume modelBounding() const { return m_modelBounding; }

        void constituteFullQuadTree(unsigned depth);

        bool isWorldPositionInside(const MathLib::Vector3& worldPosition) const;
        std::shared_ptr<QuadTreeVolume> findFittingVolume(const Engine::BoundingVolume& bv_in_world);
        std::list<std::shared_ptr<QuadTreeVolume>> collectFitVolumes(const Engine::BoundingVolume& bv_in_world);

    protected:
        SceneGraph::SpatialId subVolumeNodeId(unsigned index) const;
        bool testBoundingEnvelop(const Engine::BoundingVolume& bv_in_world) const;
        void collectFitVolumes(const Engine::BoundingVolume& bv_in_world, std::list<std::shared_ptr<QuadTreeVolume>>& result);

    protected:
        SceneGraph::SpatialId m_id;
        std::optional<SceneGraph::SpatialId> m_parentId;
        MathLib::Matrix4 m_worldTransform;
        Engine::BoundingVolume m_modelBounding;
        std::array<std::shared_ptr<QuadTreeVolume>, maxChildren> m_children;
    };
}

#endif // QUAD_TREE_VOLUME_H
