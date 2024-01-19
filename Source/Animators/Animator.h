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
#include <memory>

namespace Enigma::Animators
{
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

        const AnimatorId& id() const { return m_id; }
        virtual Engine::GenericDto serializeDto() const = 0;

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
        void setListened(bool flag) { m_isListened = flag; };

        const Engine::FactoryDesc& factoryDesc() const { return m_factoryDesc; }
        Engine::FactoryDesc& factoryDesc() { return m_factoryDesc; }

    protected:
        AnimatorId m_id;
        bool m_isListened;
        Engine::FactoryDesc m_factoryDesc;
    };
}

#endif // _ANIMATOR_H
