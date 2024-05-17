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
        QuadTreeRootDto();
        QuadTreeRootDto(const Engine::GenericDto& dto);

        [[nodiscard]] const QuadTreeRootId& id() const { return m_id; }
        void id(const QuadTreeRootId& id) { m_id = id; }
        [[nodiscard]] const SceneGraph::SpatialId& rootNodeId() const { return m_rootNodeId; }
        void rootNodeId(const SceneGraph::SpatialId& rootNodeId) { m_rootNodeId = rootNodeId; }
        [[nodiscard]] const Engine::FactoryDesc& factoryDesc() const { return m_factoryDesc; }
        void factoryDesc(const Engine::FactoryDesc& desc) { m_factoryDesc = desc; }

        Engine::GenericDto toGenericDto() const;

    protected:
        QuadTreeRootId m_id;
        SceneGraph::SpatialId m_rootNodeId;
        Engine::FactoryDesc m_factoryDesc;
    };
}

#endif // QUAD_TREE_ROOT_DTO_H
