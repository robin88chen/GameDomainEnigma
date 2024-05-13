/*****************************************************************
 * \file   QuadTreeRootDto.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   April 2024
 ******************************************************************/
#ifndef QUAD_TREE_ROOT_DTO_H
#define QUAD_TREE_ROOT_DTO_H

#include "GameEngine/GenericDto.h"
#include "QuadTreeRootId.h"
#include "SceneGraph/SpatialId.h"

namespace Enigma::WorldMap
{
    class QuadTreeRootDto
    {
    public:
        QuadTreeRootDto() = default;
        QuadTreeRootDto(const Engine::GenericDto& dto);

        [[nodiscard]] const QuadTreeRootId& id() const { return m_id; }
        void id(const QuadTreeRootId& id) { m_id = id; }
        [[nodiscard]] const SceneGraph::SpatialId& rootNodeId() const { return m_rootNodeId; }
        void rootNodeId(const SceneGraph::SpatialId& rootNodeId) { m_rootNodeId = rootNodeId; }

        Engine::GenericDto toGenericDto() const;

    protected:
        QuadTreeRootId m_id;
        SceneGraph::SpatialId m_rootNodeId;
    };
}

#endif // QUAD_TREE_ROOT_DTO_H
