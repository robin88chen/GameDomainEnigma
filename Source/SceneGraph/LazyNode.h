/*********************************************************************
 * \file   LazyNode.h
 * \brief  延遲載入的 Scene Graph Node, 先建立空間相關數據, 等可見時才載入內容
 * VisibilityManagedNode, PortalZoneNode 繼承自此
 * \author Lancelot 'Robin' Chen
 * \date   February 2023
 *********************************************************************/
#ifndef _LAZY_NODE_H
#define _LAZY_NODE_H

#include "Node.h"
#include "GameEngine/FactoryDesc.h"
#include "Frameworks/LazyStatus.h"

namespace Enigma::SceneGraph
{
    class HydratedLazyNodeAssembler;
    class HydratedLazyNodeDisassembler;

    class LazyNode : public Node
    {
        DECLARE_EN_RTTI;
    public:
        LazyNode(const SpatialId& id);
        LazyNode(const LazyNode&) = delete;
        LazyNode(LazyNode&&) = delete;
        virtual ~LazyNode() override;
        LazyNode& operator=(const LazyNode&) = delete;
        LazyNode& operator=(LazyNode&&) = delete;

        static std::shared_ptr<LazyNode> create(const SpatialId& id);

        //virtual std::error_code hydrate(const Engine::GenericDto& dto);

        virtual std::shared_ptr<SpatialAssembler> assembler() const override;
        virtual std::shared_ptr<SpatialDisassembler> disassembler() const override;
        virtual std::shared_ptr<HydratedLazyNodeAssembler> assemblerOfLaziedContent() const;
        virtual std::shared_ptr<HydratedLazyNodeDisassembler> disassemblerOfLaziedContent() const;
        virtual void assembleLaziedContent(const std::shared_ptr<HydratedLazyNodeAssembler>& assembler);
        virtual std::error_code hydrate(const std::shared_ptr<HydratedLazyNodeDisassembler>& disassembler);

        //virtual Engine::GenericDto serializeDto() override;
        //virtual Engine::GenericDto serializeLaziedContent();
        //todo : remove
        //virtual Engine::GenericDtoCollection serializeFlattenedTree() override;

        virtual bool canVisited() override;

        virtual SceneTraveler::TravelResult visitBy(SceneTraveler* traveler) override;

        const Frameworks::LazyStatus& lazyStatus() const { return m_lazyStatus; }
        Frameworks::LazyStatus& lazyStatus() { return m_lazyStatus; }

    protected:
        //virtual Engine::GenericDto serializeAsLaziness();
        //LazyNodeDto serializeLazyNodeAsLaziness();

    protected:
        Frameworks::LazyStatus m_lazyStatus;
    };
}

#endif // _LAZY_NODE_H
