/*********************************************************************
 * \file   AnimatorEvents.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   January 2024
 *********************************************************************/
#ifndef ANIMATOR_EVENTS_H
#define ANIMATOR_EVENTS_H

#include "Animator.h"
#include "AnimatorId.h"
#include "Frameworks/Event.h"
#include <system_error>

namespace Enigma::Animators
{
    class AnimatorCreated : public Frameworks::IEvent
    {
    public:
        AnimatorCreated(const AnimatorId& id, const std::shared_ptr<Animator>& animator) :
            m_id(id), m_animator(animator) {};
        const AnimatorId& id() { return m_id; }
        const std::shared_ptr<Animator>& animator() { return m_animator; }
    private:
        AnimatorId m_id;
        std::shared_ptr<Animator> m_animator;
    };
    class CreateAnimatorFailed : public Frameworks::IEvent
    {
    public:
        CreateAnimatorFailed(const AnimatorId& id, std::error_code er) :
            m_id(id), m_error(er) {};
        const AnimatorId& id() { return m_id; }
        std::error_code error() const { return m_error; }
    private:
        AnimatorId m_id;
        std::error_code m_error;
    };
    class AnimatorConstituted : public Frameworks::IEvent
    {
    public:
        AnimatorConstituted(const AnimatorId& id, const std::shared_ptr<Animator>& animator, bool is_persisted) :
            m_id(id), m_animator(animator), m_is_persisted(is_persisted) {};
        const AnimatorId& id() { return m_id; }
        const std::shared_ptr<Animator>& animator() { return m_animator; }
        bool isPersisted() const { return m_is_persisted; }
    private:
        AnimatorId m_id;
        std::shared_ptr<Animator> m_animator;
        bool m_is_persisted;
    };
    class ConstituteAnimatorFailed : public Frameworks::IEvent
    {
    public:
        ConstituteAnimatorFailed(const AnimatorId& id, std::error_code er) :
            m_id(id), m_error(er) {};
        const AnimatorId& id() { return m_id; }
        std::error_code error() const { return m_error; }
    private:
        AnimatorId m_id;
        std::error_code m_error;
    };
    class AnimatorRemoved : public Frameworks::IEvent
    {
    public:
        AnimatorRemoved(const AnimatorId& id) : m_id(id) {};
        const AnimatorId& id() { return m_id; }
    private:
        AnimatorId m_id;
    };
    class RemoveAnimatorFailed : public Frameworks::IEvent
    {
    public:
        RemoveAnimatorFailed(const AnimatorId& id, std::error_code er) :
            m_id(id), m_error(er) {};
        const AnimatorId& id() { return m_id; }
        std::error_code error() const { return m_error; }
    private:
        AnimatorId m_id;
        std::error_code m_error;
    };
    class AnimatorPut : public Frameworks::IEvent
    {
    public:
        AnimatorPut(const AnimatorId& id) : m_id(id) {};
        const AnimatorId& id() { return m_id; }
    private:
        AnimatorId m_id;
    };
    class PutAnimatorFailed : public Frameworks::IEvent
    {
    public:
        PutAnimatorFailed(const AnimatorId& id, std::error_code er) :
            m_id(id), m_error(er) {};
        const AnimatorId& id() { return m_id; }
        std::error_code error() const { return m_error; }
    private:
        AnimatorId m_id;
        std::error_code m_error;
    };
    class AnimatorReleased : public Frameworks::IEvent
    {
    public:
        AnimatorReleased(const AnimatorId& id) : m_id(id) {};
        const AnimatorId& id() { return m_id; }
    private:
        AnimatorId m_id;
    };
    class ReleaseAnimatorFailed : public Frameworks::IEvent
    {
    public:
        ReleaseAnimatorFailed(const AnimatorId& id, std::error_code er) : m_id(id), m_error(er) {};
        const AnimatorId& id() { return m_id; }
        std::error_code error() const { return m_error; }
    private:
        AnimatorId m_id;
        std::error_code m_error;
    };
}

#endif // ANIMATOR_EVENTS_H
