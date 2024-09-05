/*****************************************************************
 * \file   PortalZoneNodeAssembler.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   September 2024
 ******************************************************************/
#ifndef PORTAL_ZONE_NODE_ASSEMBLER_H
#define PORTAL_ZONE_NODE_ASSEMBLER_H

#include "LazyNodeAssembler.h"

namespace Enigma::SceneGraph
{
    class DehydratedPortalZoneNodeAssembler : public DehydratedLazyNodeAssembler
    {
    public:
        DehydratedPortalZoneNodeAssembler(const SpatialId& id);

        void portalParentId(const SpatialId& id) { m_portalParentId = id; }

        virtual Engine::GenericDto assemble() const override;

    protected:
        std::optional<SpatialId> m_portalParentId;
    };
    class HydratedPortalZoneNodeAssembler : public HydratedLazyNodeAssembler
    {
    public:
        HydratedPortalZoneNodeAssembler(const SpatialId& id);

        void portalParentId(const SpatialId& id) { m_portalParentId = id; }

        virtual Engine::GenericDto assemble() const override;

    protected:
        std::optional<SpatialId> m_portalParentId;
    };

    class DehydratedPortalZoneNodeDisassembler : public DehydratedLazyNodeDisassembler
    {
    public:
        DehydratedPortalZoneNodeDisassembler();

        [[nodiscard]] const std::optional<SpatialId>& portalParentId() const { return m_portalParentId; }

        virtual void disassemble(const Engine::GenericDto& dto) override;

    protected:
        std::optional<SpatialId> m_portalParentId;
    };
    class HydratedPortalZoneNodeDisassembler : public HydratedLazyNodeDisassembler
    {
    public:
        HydratedPortalZoneNodeDisassembler();

        [[nodiscard]] const std::optional<SpatialId>& portalParentId() const { return m_portalParentId; }

        virtual void disassemble(const Engine::GenericDto& dto) override;

    protected:
        std::optional<SpatialId> m_portalParentId;
    };
}

#endif // PORTAL_ZONE_NODE_ASSEMBLER_H
