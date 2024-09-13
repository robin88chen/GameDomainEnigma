/*****************************************************************
 * \file   OutRegionNodeAssembler.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   September 2024
 ******************************************************************/
#ifndef OUT_REGION_NODE_ASSEMBLER_H
#define OUT_REGION_NODE_ASSEMBLER_H

#include "LazyNodeAssembler.h"

namespace Enigma::SceneGraph
{
    class DehydratedOutRegionNodeAssembler : public DehydratedLazyNodeAssembler
    {
    public:
        DehydratedOutRegionNodeAssembler(const SpatialId& id);

        void ownerManagementId(const SpatialId& id) { m_ownerManagementId = id; }

        virtual Engine::GenericDto assemble() const override;

    protected:
        std::optional<SpatialId> m_ownerManagementId;
    };
    class HydratedOutRegionNodeAssembler : public HydratedLazyNodeAssembler
    {
    public:
        HydratedOutRegionNodeAssembler(const SpatialId& id);

        void ownerManagementId(const SpatialId& id) { m_ownerManagementId = id; }

        virtual Engine::GenericDto assemble() const override;

    protected:
        std::optional<SpatialId> m_ownerManagementId;
    };

    class DehydratedOutRegionNodeDisassembler : public DehydratedLazyNodeDisassembler
    {
    public:
        DehydratedOutRegionNodeDisassembler();

        [[nodiscard]] const std::optional<SpatialId>& ownerManagementId() const { return m_ownerManagementId; }

        virtual void disassemble(const Engine::GenericDto& dto) override;

    protected:
        std::optional<SpatialId> m_ownerManagementId;
    };
    class HydratedOutRegionNodeDisassembler : public HydratedLazyNodeDisassembler
    {
    public:
        HydratedOutRegionNodeDisassembler();

        [[nodiscard]] const std::optional<SpatialId>& ownerManagementId() const { return m_ownerManagementId; }

        virtual void disassemble(const Engine::GenericDto& dto) override;

    protected:
        std::optional<SpatialId> m_ownerManagementId;
    };
}

#endif // OUT_REGION_NODE_ASSEMBLER_H
