/*****************************************************************
 * \file   PortalManagementNodeAssembler.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   September 2024
 ******************************************************************/
#ifndef PORTAL_MANAGEMENT_NODE_ASSEMBLER_H
#define PORTAL_MANAGEMENT_NODE_ASSEMBLER_H

#include "NodeAssembler.h"

namespace Enigma::SceneGraph
{
    class PortalManagementNode;
    class PortalManagementNodeAssembler : public NodeAssembler
    {
    public:
        PortalManagementNodeAssembler(const SpatialId& id);

        void outsideRegionId(const SpatialId& outside_id) { m_outsideRegionId = outside_id; }

        virtual Engine::GenericDto assemble() const override;

    protected:
        std::optional<SpatialId> m_outsideRegionId;
    };
    class PortalManagementNodeDisassembler : public NodeDisassembler
    {
    public:
        PortalManagementNodeDisassembler();

        [[nodiscard]] const std::optional<SpatialId>& outsideRegionId() const { return m_outsideRegionId; }

        virtual void disassemble(const Engine::GenericDto& dto) override;

    protected:
        std::optional<SpatialId> m_outsideRegionId;
    };
}

#endif // PORTAL_MANAGEMENT_NODE_ASSEMBLER_H
