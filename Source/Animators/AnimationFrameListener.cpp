#include "AnimationFrameListener.h"
#include "AnimatorErrors.h"
#include <cassert>

using namespace Enigma::Frameworks;
using namespace Enigma::Engine;
using namespace Enigma::Animators;

DEFINE_RTTI(Animators, AnimationFrameListener, ISystemService);

AnimationFrameListener::AnimationFrameListener(Frameworks::ServiceManager* manager, Engine::TimerService* timer) : ISystemService(manager)
{
    assert(timer);
    m_timer = timer;
    m_needTick = false;
}

AnimationFrameListener::~AnimationFrameListener()
{

}

ServiceResult AnimationFrameListener::OnInit()
{
    return ServiceResult::Complete;
}

ServiceResult AnimationFrameListener::OnTick()
{
    if (m_timer)
    {
        UpdateAnimator(m_timer->GetGameTimer());
    }
    if (m_hasExpiredAnimator) RemoveExpiredAnimator();
    return ServiceResult::Pendding;
}

error AnimationFrameListener::AddListeningAnimator(const Engine::AnimatorPtr& ani)
{
    if (!ani) return ErrorCode::nullAnimator;
    if (ani->IsListened()) return ErrorCode::animatorMultiListening;
    m_listeningAnimators.emplace_back(ani);
    ani->SetListened(true);
    ani->ProcessAfterAddListening();

    m_needTick = true;
    return ErrorCode::ok;
}

error AnimationFrameListener::RemoveListeningAnimator(const Engine::AnimatorPtr& ani)
{
    if (!ani) return ErrorCode::nullAnimator;
    if (!ani->IsListened()) return ErrorCode::ok;

    ani->SetListened(false);
    ani->ProcessBeforeRemoveListening();
    m_listeningAnimators.remove_if([=](std::weak_ptr<Animator> wp)
        { return ((!wp.expired()) && (wp.lock() == ani)); });
    if (m_listeningAnimators.empty())
    {
        m_needTick = false;
    }
    return ErrorCode::ok;
}

bool AnimationFrameListener::UpdateAnimator(const std::unique_ptr<Frameworks::Timer>& timer)
{
    if (!timer) return false;

    bool all_res = false;
    ListeningList::iterator iter = m_listeningAnimators.begin();
    while (iter != m_listeningAnimators.end())
    {
        if (iter->expired())
        {
            m_hasExpiredAnimator = true;
            ++iter;
            continue;
        }
        const AnimatorPtr& ani = iter->lock();
        if (!ani)
        {
            ++iter;
            continue;
        }

        bool res = (bool)(ani->Update(timer));
        all_res |= res;
        if (!res)  // no update, remove this animator and continue
        {
            ani->SetListened(false);
            ani->ProcessBeforeRemoveListening();
            m_hasExpiredAnimator = true;
            ++iter;
        }
        else
        {
            ++iter;
        }
    }
    return all_res;
}

void AnimationFrameListener::RemoveExpiredAnimator()
{
    if (!m_hasExpiredAnimator) return;
    m_listeningAnimators.remove_if([=](std::weak_ptr<Animator> wp)
        { return (wp.expired()) || (!wp.lock()->IsListened()); });
    m_hasExpiredAnimator = false;
    if (m_listeningAnimators.empty())
    {
        m_needTick = false;
    }
}
