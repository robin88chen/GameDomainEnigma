/*********************************************************************
 * \file   PortalZoneNode.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   March 2023
 *********************************************************************/
#ifndef _PORTAL_ZONE_NODE_H
#define _PORTAL_ZONE_NODE_H

#include "LazyNode.h"
#include <memory>

namespace Enigma::SceneGraph
{
    class SceneGraphRepository;
    class PortalZoneNode : public LazyNode
    {
        DECLARE_EN_RTTI;
    public:
        PortalZoneNode(const SpatialId& id);
        PortalZoneNode(const PortalZoneNode&) = delete;
        PortalZoneNode(PortalZoneNode&&) = delete;
        PortalZoneNode& operator=(const PortalZoneNode&) = delete;
        PortalZoneNode& operator=(PortalZoneNode&&) = delete;
        virtual ~PortalZoneNode() override;

        static std::shared_ptr<PortalZoneNode> create(const SpatialId& id);

        virtual std::shared_ptr<SpatialAssembler> assembler() const override;
        virtual void assemble(const std::shared_ptr<SpatialAssembler>& assembler) override;
        virtual std::shared_ptr<SpatialDisassembler> disassembler() const override;
        virtual void disassemble(const std::shared_ptr<SpatialDisassembler>& disassembler) override;
        virtual std::shared_ptr<HydratedLazyNodeAssembler> assemblerOfLaziedContent() const override;
        virtual std::shared_ptr<HydratedLazyNodeDisassembler> disassemblerOfLaziedContent() const override;
        virtual void assembleLaziedContent(const std::shared_ptr<HydratedLazyNodeAssembler>& assembler) override;

        virtual error onCullingVisible(Culler* culler, bool noCull) override;

        void parentPortal(const SpatialId& id);
        const std::optional<SpatialId>& parentPortal() const;

    protected:
        std::optional<SpatialId> m_portalParentId; // either portal or portal management node
        bool m_hasTraversed;
    };
}

#endif // _PORTAL_ZONE_NODE_H
