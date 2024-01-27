/*********************************************************************
 * \file   AnimationFrameListener.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   January 2023
 *********************************************************************/
#ifndef _ANIMATION_FRAME_LISTENER_H
#define _ANIMATION_FRAME_LISTENER_H

#include "Frameworks/SystemService.h"
#include "Frameworks/ServiceManager.h"
#include "GameEngine/TimerService.h"
#include "Animator.h"
#include "Frameworks/CommandSubscriber.h"
#include <system_error>
#include <memory>

namespace Enigma::Animators
{
    using error = std::error_code;
    class AnimatorRepository;

    class AnimationFrameListener : public Frameworks::ISystemService
    {
        DECLARE_EN_RTTI;
    public:
        AnimationFrameListener(Frameworks::ServiceManager* manager, const std::shared_ptr<AnimatorRepository>& repository, const std::shared_ptr<Engine::TimerService>& timer);
        AnimationFrameListener(const AnimationFrameListener&) = delete;
        AnimationFrameListener(AnimationFrameListener&&) = delete;
        ~AnimationFrameListener() override;
        AnimationFrameListener& operator=(const AnimationFrameListener&) = delete;
        AnimationFrameListener& operator=(AnimationFrameListener&&) = delete;

        virtual Frameworks::ServiceResult onInit() override;
        virtual Frameworks::ServiceResult onTick() override;
        virtual Frameworks::ServiceResult onTerm() override;

        error addListeningAnimator(const AnimatorId& animator_id);
        error removeListeningAnimator(const AnimatorId& animator_id);
        /** update listened animator
        @return true: some animator has update, false: no update */
        bool updateAnimator(const std::unique_ptr<Frameworks::Timer>& timer);

    private:
        void removeExpiredAnimator();

        void addListeningAnimator(const Frameworks::ICommandPtr& c);
        void removeListeningAnimator(const Frameworks::ICommandPtr& c);

    private:
        std::weak_ptr<AnimatorRepository> m_repository;
        std::weak_ptr<Engine::TimerService> m_timer;

        using ListeningList = std::list<std::weak_ptr<Animator>>;
        ListeningList m_listeningAnimators;
        bool m_hasExpiredAnimator;

        Frameworks::CommandSubscriberPtr m_addListeningAnimator;
        Frameworks::CommandSubscriberPtr m_removeListeningAnimator;
    };
}

#endif // _ANIMATION_FRAME_LISTENER_H
