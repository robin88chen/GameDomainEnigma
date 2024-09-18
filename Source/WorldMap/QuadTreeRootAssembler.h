/*********************************************************************
 * \file   QuadTreeRootAssembler.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   September 2024
 *********************************************************************/
#ifndef QUAD_TREE_ROOT_ASSEMBLER_H
#define QUAD_TREE_ROOT_ASSEMBLER_H

#include "QuadTreeRootId.h"
#include "GameEngine/GenericDto.h"
#include "SceneGraph/SpatialId.h"
#include "GameEngine/FactoryDesc.h"

namespace Enigma::WorldMap
{
    class QuadTreeRootAssembler
    {
    public:
        QuadTreeRootAssembler(const QuadTreeRootId& id);
        virtual ~QuadTreeRootAssembler() = default;

        void rootNodeId(const SceneGraph::SpatialId& id) { m_rootNodeId = id; }
        void factoryDesc(const Engine::FactoryDesc& desc) { m_factoryDesc = desc; }

        Engine::GenericDto assemble() const;

    protected:
        QuadTreeRootId m_id;
        SceneGraph::SpatialId m_rootNodeId;
        Engine::FactoryDesc m_factoryDesc;
    };

    class QuadTreeRootDisassembler
    {
    public:
        QuadTreeRootDisassembler();
        virtual ~QuadTreeRootDisassembler() = default;

        void disassemble(const Engine::GenericDto& dto);

        [[nodiscard]] const QuadTreeRootId& id() const { return m_id; }
        [[nodiscard]] const SceneGraph::SpatialId& rootNodeId() const { return m_rootNodeId; }
        [[nodiscard]] const Engine::FactoryDesc& factoryDesc() const { return m_factoryDesc; }

    protected:
        QuadTreeRootId m_id;
        SceneGraph::SpatialId m_rootNodeId;
        Engine::FactoryDesc m_factoryDesc;
    };
}

#endif // QUAD_TREE_ROOT_ASSEMBLER_H
