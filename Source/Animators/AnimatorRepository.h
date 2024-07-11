/*********************************************************************
 * \file   AnimatorRepository.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   January 2024
 *********************************************************************/
#ifndef ANIMATOR_REPOSITORY_H
#define ANIMATOR_REPOSITORY_H

#include "Frameworks/SystemService.h"
#include "Frameworks/QuerySubscriber.h"
#include "Frameworks/CommandSubscriber.h"
#include "AnimatorId.h"
#include "AnimatorFactoryDelegate.h"
#include <mutex>

namespace Enigma::Animators
{
    class Animator;
    class AnimatorStoreMapper;
    class AnimatorFactory;

    class AnimatorRepository : public Frameworks::ISystemService
    {
        DECLARE_EN_RTTI;
    public:
        AnimatorRepository(Frameworks::ServiceManager* srv_manager, const std::shared_ptr<AnimatorStoreMapper>& store_mapper);
        AnimatorRepository(const AnimatorRepository&) = delete;
        AnimatorRepository(AnimatorRepository&&) = delete;
        AnimatorRepository& operator=(const AnimatorRepository&) = delete;
        AnimatorRepository& operator=(AnimatorRepository&&) = delete;
        ~AnimatorRepository() override;

        virtual Frameworks::ServiceResult onInit() override;
        virtual Frameworks::ServiceResult onTerm() override;

        void registerAnimatorFactory(const std::string& rtti, const AnimatorCreator& creator, const AnimatorConstitutor& constitutor);

        std::uint64_t nextSequenceNumber();
        bool hasAnimator(const AnimatorId& id);
        std::shared_ptr<Animator> queryAnimator(const AnimatorId& id);
        void removeAnimator(const AnimatorId& id);
        void putAnimator(const AnimatorId& id, const std::shared_ptr<Animator>& animator);
        void releaseAnimator(const AnimatorId& id);

    protected:
        void queryAnimator(const Frameworks::IQueryPtr& q);
        void queryAnimatorNextSequenceNumber(const Frameworks::IQueryPtr& q);
        void requestAnimatorCreation(const Frameworks::IQueryPtr& r);
        void requestAnimatorConstitution(const Frameworks::IQueryPtr& r);
        void putAnimator(const Frameworks::ICommandPtr& c);
        void removeAnimator(const Frameworks::ICommandPtr& c);
        void releaseAnimator(const Frameworks::ICommandPtr& c);

    protected:
        std::shared_ptr<AnimatorStoreMapper> m_storeMapper;
        AnimatorFactory* m_factory;

        std::unordered_map<AnimatorId, std::shared_ptr<Animator>, AnimatorId::hash> m_animators;
        std::recursive_mutex m_animatorLock;

        Frameworks::QuerySubscriberPtr m_queryAnimator;
        Frameworks::QuerySubscriberPtr m_queryAnimatorNextSequenceNumber;
        Frameworks::QuerySubscriberPtr m_requestAnimatorCreation;
        Frameworks::QuerySubscriberPtr m_requestAnimatorConstitution;

        Frameworks::CommandSubscriberPtr m_putAnimator;
        Frameworks::CommandSubscriberPtr m_removeAnimator;
        Frameworks::CommandSubscriberPtr m_releaseAnimator;
    };
}

#endif // ANIMATOR_REPOSITORY_H
