/*********************************************************************
 * \file   OutRegionNode.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   July 2024
 *********************************************************************/
#ifndef OUT_REGION_NODE_H
#define OUT_REGION_NODE_H

#include "LazyNode.h"

namespace Enigma::SceneGraph
{
    class OutRegionNode : public LazyNode
    {
        DECLARE_EN_RTTI;
    public:
        OutRegionNode(const SpatialId& id);
        OutRegionNode(const OutRegionNode&) = delete;
        OutRegionNode(OutRegionNode&&) = delete;
        OutRegionNode& operator=(const OutRegionNode&) = delete;
        OutRegionNode& operator=(OutRegionNode&&) = delete;
        virtual ~OutRegionNode() override;

        static std::shared_ptr<OutRegionNode> create(const SpatialId& id);

        virtual std::shared_ptr<SpatialAssembler> assembler() const override;
        virtual void assemble(const std::shared_ptr<SpatialAssembler>& assembler) override;
        virtual std::shared_ptr<SpatialDisassembler> disassembler() const override;
        virtual void disassemble(const std::shared_ptr<SpatialDisassembler>& disassembler) override;
        virtual std::shared_ptr<HydratedLazyNodeAssembler> assemblerOfLaziedContent() const override;
        virtual std::shared_ptr<HydratedLazyNodeDisassembler> disassemblerOfLaziedContent() const override;
        virtual void assembleLaziedContent(const std::shared_ptr<HydratedLazyNodeAssembler>& assembler) override;

        virtual error onCullingVisible(Culler* culler, bool noCull) override;

        void ownerManagementNode(const SpatialId& id);
        const std::optional<SpatialId>& ownerManagementNode() const;

    protected:
        std::optional<SpatialId> m_ownerManagementId;
        bool m_hasTraversed;
    };
}

#endif // OUT_REGION_NODE_H
