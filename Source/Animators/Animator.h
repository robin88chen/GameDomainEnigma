/*********************************************************************
 * \file   Animator.h
 * \brief  animator base class, entity, use shared_ptr
 *
 * \author Lancelot 'Robin' Chen
 * \date   January 2023
 *********************************************************************/
#ifndef _ANIMATOR_H
#define _ANIMATOR_H

#include "Frameworks/Rtti.h"
#include "Frameworks/Timer.h"
#include "GameEngine/FactoryDesc.h"
#include "GameEngine/GenericDto.h"
#include "AnimatorId.h"
#include "Primitives/PrimitiveId.h"
#include <memory>

namespace Enigma::Animators
{
    class AnimatorAssembler;
    class AnimatorDisassembler;

    class Animator : public std::enable_shared_from_this<Animator>
    {
        DECLARE_EN_RTTI_OF_BASE
    public:
        enum class HasUpdated : bool
        {
            False,
            True
        };
    public:
        Animator(const AnimatorId& id);
        Animator(const Animator& ani) = delete;
        Animator(Animator&& ani) noexcept = delete;
        virtual ~Animator();
        Animator& operator=(const Animator&) = delete;
        Animator& operator=(Animator&&) noexcept = delete;

        static std::shared_ptr<Animator> queryAnimator(const AnimatorId& id);

        virtual std::shared_ptr<AnimatorAssembler> assembler() const = 0;
        virtual void assemble(const std::shared_ptr<AnimatorAssembler>& assembler) const = 0; ///< that's double dispatch
        virtual std::shared_ptr<AnimatorDisassembler> disassembler() const = 0;
        virtual void disassemble(const std::shared_ptr<AnimatorDisassembler>& disassembler) = 0; ///< that's double dispatch
        const AnimatorId& id() const { return m_id; }

        std::optional<Primitives::PrimitiveId> controlledPrimitiveId() const { return m_controlledPrimitiveId; }
        std::optional<Primitives::PrimitiveId>& controlledPrimitiveId() { return m_controlledPrimitiveId; }

        /** animation update
        @return has update something or not */
        virtual HasUpdated update(const std::unique_ptr<Frameworks::Timer>&) { return HasUpdated::False; /* base class do nothing */ };
        /** reset animation */
        virtual void reset() {};

        /** called after animator add to listening list */
        virtual void processAfterAddListening() {};
        /** called before animator remove from listening list */
        virtual void processBeforeRemoveListening() {};

        bool isListened() const { return m_isListened; };
        void isListened(bool flag) { m_isListened = flag; };

        const Engine::FactoryDesc& factoryDesc() const { return m_factoryDesc; }
        Engine::FactoryDesc& factoryDesc() { return m_factoryDesc; }

    protected:
        AnimatorId m_id;
        bool m_isListened;
        Engine::FactoryDesc m_factoryDesc;
        std::optional<Primitives::PrimitiveId> m_controlledPrimitiveId;
    };
}

#endif // _ANIMATOR_H
