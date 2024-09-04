/*********************************************************************
 * \file   VisibilityManagedNode.h
 * \brief  開闊世界地形上，以可見範圍控制的 Lazy Node,
 *         類似 Portal Zone Node, 但增加了釋放機制
 *         會用在廣大的世界地圖上
 * \author Lancelot 'Robin' Chen
 * \date   March 2023
 *********************************************************************/
#ifndef _VISIBILITY_MANAGED_NODE_H
#define _VISIBILITY_MANAGED_NODE_H

#include "LazyNode.h"

namespace Enigma::SceneGraph
{
    class VisibilityManagedNode : public LazyNode
    {
        DECLARE_EN_RTTI;
    public:
        VisibilityManagedNode(const SpatialId& id);
        //VisibilityManagedNode(const SpatialId& id, const Engine::GenericDto& o);
        VisibilityManagedNode(const VisibilityManagedNode&) = delete;
        VisibilityManagedNode(VisibilityManagedNode&&) = delete;
        virtual ~VisibilityManagedNode() override;
        VisibilityManagedNode& operator=(const VisibilityManagedNode&) = delete;
        VisibilityManagedNode& operator=(VisibilityManagedNode&&) = delete;

        virtual std::shared_ptr<SpatialAssembler> assembler() const override;
        virtual std::shared_ptr<SpatialDisassembler> disassembler() const override;
        virtual std::shared_ptr<HydratedLazyNodeAssembler> assemblerOfLaziedContent() const override;
        virtual std::shared_ptr<HydratedLazyNodeDisassembler> disassemblerOfLaziedContent() const override;
        //static std::shared_ptr<VisibilityManagedNode> create(const SpatialId& id);
        //static std::shared_ptr<VisibilityManagedNode> constitute(const SpatialId& id, const Engine::GenericDto& dto);

        //virtual Engine::GenericDto serializeDto() override;

        virtual error onCullingVisible(Culler* culler, bool noCull) override;
        virtual void onCullingCompleteNotVisible(Culler* culler) override;

        void dehydrate();
    };
}

#endif // _VISIBILITY_MANAGED_NODE_H
