/*********************************************************************
 * \file   AnimatorEvents.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   January 2023
 *********************************************************************/
#ifndef _ANIMATOR_EVENTS_H
#define _ANIMATOR_EVENTS_H

#include "Frameworks/ruid.h"
#include <memory>

namespace Enigma::Animators
{
    class ModelPrimitiveAnimator;

    class ModelAnimatorBuilt : public Frameworks::IEvent
    {
    public:
        ModelAnimatorBuilt(const Frameworks::Ruid& ruid, const std::shared_ptr<ModelPrimitiveAnimator>& ani) :
            m_ruid(ruid), m_animator(ani) {};
        const Frameworks::Ruid& getRuid() { return m_ruid; }
        const std::shared_ptr<ModelPrimitiveAnimator>& getAnimator() { return m_animator; }
    private:
        Frameworks::Ruid m_ruid;
        std::shared_ptr<ModelPrimitiveAnimator> m_animator;
    };
    class BuildModelAnimatorFailed : public Frameworks::IEvent
    {
    public:
        BuildModelAnimatorFailed(const Frameworks::Ruid& ruid, std::error_code er) :
            m_ruid(ruid), m_error(er) {};
        const Frameworks::Ruid& getRuid() { return m_ruid; }
        std::error_code GetErrorCode() const { return m_error; }
    private:
        Frameworks::Ruid m_ruid;
        std::error_code m_error;
    };

}

#endif // _ANIMATOR_EVENTS_H
