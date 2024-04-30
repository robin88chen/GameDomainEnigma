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
    class QuadTreeVolume
    {
    public:
        QuadTreeVolume(const SceneGraph::SpatialId& id);

        const SceneGraph::SpatialId& id() const { return m_id; }
        std::shared_ptr<QuadTreeVolume> findFittingVolume(const Engine::BoundingVolume& bv_in_world);

    protected:
        SceneGraph::SpatialId subVolumeNodeId(unsigned index);
    protected:
        static constexpr std::size_t maxChildren = 4;
        SceneGraph::SpatialId m_id;
        std::shared_ptr<SceneGraph::Node> m_node;
        std::array<std::shared_ptr<QuadTreeVolume>, maxChildren> m_children;
    };
}

#endif // QUAD_TREE_VOLUME_H
