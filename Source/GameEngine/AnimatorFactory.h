/*********************************************************************
 * \file   AnimatorFactory.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   January 2024
 *********************************************************************/
#ifndef ANIMATOR_FACTORY_H
#define ANIMATOR_FACTORY_H

#include "GenericDto.h"
#include "AnimatorId.h"
#include "Frameworks/CommandSubscriber.h"
#include <unordered_map>

namespace Enigma::Engine
{
    class Animator;
    using AnimatorCreator = std::function<std::shared_ptr<Animator>(const AnimatorId& id)>;
    using AnimatorConstitutor = std::function<std::shared_ptr<Animator>(const AnimatorId& id, const Engine::GenericDto& dto)>;

    class AnimatorFactory
    {
    public:
        AnimatorFactory();
        ~AnimatorFactory();

        void registerHandlers();
        void unregisterHandlers();

        std::shared_ptr<Animator> create(const AnimatorId& id, const Frameworks::Rtti& rtti);
        std::shared_ptr<Animator> constitute(const AnimatorId& id, const Engine::GenericDto& dto, bool is_persisted);

        void registerAnimatorFactory(const std::string& rtti, const AnimatorCreator& creator, const AnimatorConstitutor& constitutor);
        void unregisterAnimatorFactory(const std::string& rtti);

    private:
        void registerAnimatorFactory(const Frameworks::ICommandPtr& c);
        void unregisterAnimatorFactory(const Frameworks::ICommandPtr& c);
        void createAnimator(const Frameworks::ICommandPtr& c);
        void constituteAnimator(const Frameworks::ICommandPtr& c);

    private:
        std::unordered_map<std::string, AnimatorCreator> m_creators; // rtti name -> creator
        std::unordered_map<std::string, AnimatorConstitutor> m_constitutors; // rtti name -> constitutor

        Frameworks::CommandSubscriberPtr m_registerAnimatorFactory;
        Frameworks::CommandSubscriberPtr m_unregisterAnimatorFactory;
        Frameworks::CommandSubscriberPtr m_createAnimator;
        Frameworks::CommandSubscriberPtr m_constituteAnimator;
    };
}

#endif // ANIMATOR_FACTORY_H
