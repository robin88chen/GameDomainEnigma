/*********************************************************************
 * \file   LazyNodeAssembler.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   September 2024
 *********************************************************************/
#ifndef LAZY_NODE_ASSEMBLER_H
#define LAZY_NODE_ASSEMBLER_H

#include "NodeAssembler.h"

namespace Enigma::SceneGraph
{
    class LazyNodeAssembler : public NodeAssembler
    {
    public:
        virtual void factory(const Engine::FactoryDesc& factory) override;
        virtual void persist(const std::string& filename, const std::string& path_id) override;

    protected:
        LazyNodeAssembler(const SpatialId& id);
    };

    class DehydratedLazyNodeAssembler : public LazyNodeAssembler
    {
    public:
        DehydratedLazyNodeAssembler(const SpatialId& id) : LazyNodeAssembler(id) {}

        virtual Engine::GenericDto assemble() const override;
    };

    class HydratedLazyNodeAssembler : public LazyNodeAssembler
    {
    public:
        HydratedLazyNodeAssembler(const SpatialId& id) : LazyNodeAssembler(id) {}
    };

    class LazyNodeDisassembler : public NodeDisassembler
    {
    protected:
        LazyNodeDisassembler() : NodeDisassembler() {}
        virtual ~LazyNodeDisassembler() override = default;
    };

    class DehydratedLazyNodeDisassembler : public LazyNodeDisassembler
    {
    public:
        DehydratedLazyNodeDisassembler() : LazyNodeDisassembler() {}
        virtual ~DehydratedLazyNodeDisassembler() override = default;
    };

    class HydratedLazyNodeDisassembler : public LazyNodeDisassembler
    {
    public:
        HydratedLazyNodeDisassembler() : LazyNodeDisassembler() {}
        virtual ~HydratedLazyNodeDisassembler() override = default;
    };
}

#endif // LAZY_NODE_ASSEMBLER_H
