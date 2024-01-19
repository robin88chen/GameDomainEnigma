#include "AnimationFrameListener.h"
#include "AnimatorErrors.h"
#include "AnimatorCommands.h"
#include "Frameworks/CommandBus.h"
#include "GameEngine/TimerService.h"
#include <cassert>

using namespace Enigma::Frameworks;
using namespace Enigma::Animators;

DEFINE_RTTI(Animators, AnimationFrameListener, ISystemService);

AnimationFrameListener::AnimationFrameListener(ServiceManager* manager, const std::shared_ptr<Engine::TimerService>& timer)
    : ISystemService(manager), m_hasExpiredAnimator(false)
{
    assert(timer);
    m_timer = timer;
    m_needTick = false;
}

AnimationFrameListener::~AnimationFrameListener()
{

}

ServiceResult AnimationFrameListener::onInit()
{
    m_addListeningAnimator = std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { this->addListeningAnimator(c); });
    m_removeListeningAnimator = std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { this->removeListeningAnimator(c); });
    CommandBus::subscribe(typeid(AddListeningAnimator), m_addListeningAnimator);
    CommandBus::subscribe(typeid(RemoveListeningAnimator), m_removeListeningAnimator);
    return ServiceResult::Complete;
}

ServiceResult AnimationFrameListener::onTick()
{
    if (!m_timer.expired())
    {
        updateAnimator(m_timer.lock()->GetGameTimer());
    }
    if (m_hasExpiredAnimator) removeExpiredAnimator();
    return ServiceResult::Pendding;
}

ServiceResult AnimationFrameListener::onTerm()
{
    CommandBus::unsubscribe(typeid(AddListeningAnimator), m_addListeningAnimator);
    CommandBus::unsubscribe(typeid(RemoveListeningAnimator), m_removeListeningAnimator);
    m_addListeningAnimator = nullptr;
    m_removeListeningAnimator = nullptr;
    return ServiceResult::Complete;
}

error AnimationFrameListener::addListeningAnimator(const std::shared_ptr<Animator>& ani)
{
    if (!ani) return ErrorCode::nullAnimator;
    if (ani->isListened()) return ErrorCode::animatorMultiListening;
    m_listeningAnimators.emplace_back(ani);
    ani->setListened(true);
    ani->processAfterAddListening();

    m_needTick = true;
    return ErrorCode::ok;
}

error AnimationFrameListener::removeListeningAnimator(const std::shared_ptr<Animator>& ani)
{
    if (!ani) return ErrorCode::nullAnimator;
    if (!ani->isListened()) return ErrorCode::ok;

    ani->setListened(false);
    ani->processBeforeRemoveListening();
    m_listeningAnimators.remove_if([=](std::weak_ptr<Animator> wp)
        { return ((!wp.expired()) && (wp.lock() == ani)); });
    if (m_listeningAnimators.empty())
    {
        m_needTick = false;
    }
    return ErrorCode::ok;
}

bool AnimationFrameListener::updateAnimator(const std::unique_ptr<Timer>& timer)
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
        const std::shared_ptr<Animator>& ani = iter->lock();
        if (!ani)
        {
            ++iter;
            continue;
        }

        bool res = static_cast<bool>(ani->update(timer));
        all_res |= res;
        if (!res)  // no update, remove this animator and continue
        {
            ani->setListened(false);
            ani->processBeforeRemoveListening();
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

void AnimationFrameListener::removeExpiredAnimator()
{
    if (!m_hasExpiredAnimator) return;
    m_listeningAnimators.remove_if([=](std::weak_ptr<Animator> wp)
        { return (wp.expired()) || (!wp.lock()->isListened()); });
    m_hasExpiredAnimator = false;
    if (m_listeningAnimators.empty())
    {
        m_needTick = false;
    }
}

void AnimationFrameListener::addListeningAnimator(const ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<AddListeningAnimator, ICommand>(c);
    if (!cmd) return;
    addListeningAnimator(cmd->animator());
}

void AnimationFrameListener::removeListeningAnimator(const ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<RemoveListeningAnimator, ICommand>(c);
    if (!cmd) return;
    removeListeningAnimator(cmd->animator());
}
