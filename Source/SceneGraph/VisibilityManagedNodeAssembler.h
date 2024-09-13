/*********************************************************************
 * \file   VisibilityManagedNodeAssembler.h
 * \brief  ADR : 概念問題, 雖然組裝的實作與LazyNode 相同, 但要表達 --
 *   它是屬於 VisibilityManagedNode 的組裝器
 * \author Lancelot 'Robin' Chen
 * \date   September 2024
 *********************************************************************/
#ifndef VISIBILITY_MANAGED_NODE_ASSEMBLER_H
#define VISIBILITY_MANAGED_NODE_ASSEMBLER_H

#include "LazyNodeAssembler.h"

namespace Enigma::SceneGraph
{
    class DehydratedVisibilityManagedNodeAssembler : public DehydratedLazyNodeAssembler
    {
    public:
        DehydratedVisibilityManagedNodeAssembler(const SpatialId& id);
    };
    class HydratedVisibilityManagedNodeAssembler : public HydratedLazyNodeAssembler
    {
    public:
        HydratedVisibilityManagedNodeAssembler(const SpatialId& id);
    };
    class DehydratedVisibilityManagedNodeDisassembler : public DehydratedLazyNodeDisassembler
    {
    public:
        DehydratedVisibilityManagedNodeDisassembler();
    };
    class HydratedVisibilityManagedNodeDisassembler : public HydratedLazyNodeDisassembler
    {
    public:
        HydratedVisibilityManagedNodeDisassembler();
    };
}

#endif // VISIBILITY_MANAGED_NODE_ASSEMBLER_H
