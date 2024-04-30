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

namespace Enigma::WorldMap
{
    class QuadTreeRoot
    {
    public:
        QuadTreeRoot(const QuadTreeRootId& id);
        QuadTreeRoot(const QuadTreeRootId& id, const Engine::GenericDto& o);

        const QuadTreeRootId& id() const { return m_id; }
        const SceneGraph::SpatialId& rootNodeId() const { return m_rootNodeId; }

    protected:
        QuadTreeRootId m_id;
        SceneGraph::SpatialId m_rootNodeId;
    };
}

#endif // QUAD_TREE_ROOT_H
