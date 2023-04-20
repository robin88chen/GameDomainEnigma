/*********************************************************************
 * \file   Animator.h
 * \brief  animator base class, value object, use shared_ptr
 *
 * \author Lancelot 'Robin' Chen
 * \date   January 2023
 *********************************************************************/
#ifndef _ANIMATOR_H
#define _ANIMATOR_H

#include "Frameworks/Rtti.h"
#include "Frameworks/Timer.h"
#include <memory>

namespace Enigma::Engine
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
        Animator();
        Animator(const Animator& ani);
        Animator(Animator&& ani);
        virtual ~Animator();
        Animator& operator=(const Animator&);
        Animator& operator=(Animator&&);

        /** animation update
        @return has update something or not */
        virtual HasUpdated Update(const std::unique_ptr<Frameworks::Timer>&) { return HasUpdated::False; /* base class do nothing */ };
        /** reset animation */
        virtual void Reset() {};

        /** called after animator add to listening list */
        virtual void ProcessAfterAddListening() {};
        /** called before animator remove from listening list */
        virtual void ProcessBeforeRemoveListening() {};

        bool IsListened() { return m_isListened; };
        void SetListened(bool flag) { m_isListened = flag; };

    protected:
        bool m_isListened;
    };
    using AnimatorPtr = std::shared_ptr<Animator>;
}

#endif // _ANIMATOR_H
