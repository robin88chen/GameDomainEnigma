/*****************************************************************
 * \file   PortalAssembler.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   September 2024
 ******************************************************************/
#ifndef PORTAL_ASSEMBLER_H
#define PORTAL_ASSEMBLER_H

#include "SpatialAssembler.h"

namespace Enigma::SceneGraph
{
    class PortalAssembler : public SpatialAssembler
    {
    public:
        PortalAssembler(const SpatialId& id);

        void adjacentNodeId(const SpatialId& id) { m_adjacentNodeId = id; }
        void isOpen(bool open) { m_isOpen = open; }

        virtual Engine::GenericDto assemble() const override;

    protected:
        std::optional<SpatialId> m_adjacentNodeId;
        bool m_isOpen;
    };

    class PortalDisassembler : public SpatialDisassembler
    {
    public:
        PortalDisassembler();

        [[nodiscard]] const std::optional<SpatialId>& adjacentNodeId() const { return m_adjacentNodeId; }
        [[nodiscard]] bool isOpen() const { return m_isOpen; }

        virtual void disassemble(const Engine::GenericDto& dto) override;

    protected:
        std::optional<SpatialId> m_adjacentNodeId;
        bool m_isOpen;
    };
}

#endif // PORTAL_ASSEMBLER_H
