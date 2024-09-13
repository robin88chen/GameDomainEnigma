/*********************************************************************
 * \file   NodeAssembler.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   September 2024
 *********************************************************************/
#ifndef NODE_ASSEMBLER_H
#define NODE_ASSEMBLER_H

#include "SpatialAssembler.h"
#include "Spatial.h"

namespace Enigma::SceneGraph
{
    class NodeAssembler : public SpatialAssembler
    {
    public:
        NodeAssembler(const SpatialId& id);

        void child(const SpatialId& child_id);
        void child(const std::shared_ptr<SpatialAssembler>& child);
        void child(const std::shared_ptr<Spatial>& child);

        virtual Engine::GenericDto assemble() const override;

    protected:
        void consistChildrenLocationBounding();

    protected:
        std::vector<std::tuple<SpatialId, std::shared_ptr<SpatialAssembler>>> m_children;
    };

    class NodeDisassembler : public SpatialDisassembler
    {
    public:
        NodeDisassembler();

        [[nodiscard]] const std::vector<std::tuple<SpatialId, std::optional<Engine::GenericDto>>>& children() const { return m_children; }
        virtual void disassemble(const Engine::GenericDto& dto) override;

    protected:
        std::vector<std::tuple<SpatialId, std::optional<Engine::GenericDto>>> m_children;
    };
}

#endif // NODE_ASSEMBLER_H

