#include "AnimatorRepository.h"
#include "AnimatorStoreMapper.h"
#include "AnimatorFactory.h"
#include "AnimatorQueries.h"
#include "AnimatorCommands.h"
#include "Frameworks/QueryDispatcher.h"
#include "Frameworks/CommandBus.h"
#include "Platforms/MemoryMacro.h"
#include "AnimatorEvents.h"
#include "Frameworks/EventPublisher.h"
#include "Platforms/PlatformLayer.h"
#include <cassert>

using namespace Enigma::Animators;
using namespace Enigma::Frameworks;

DEFINE_RTTI(Animators, AnimatorRepository, ISystemService);

using error = std::error_code;

AnimatorRepository::AnimatorRepository(Frameworks::ServiceManager* srv_manager, const std::shared_ptr<AnimatorStoreMapper>& store_mapper) : ISystemService(srv_manager)
{
    m_factory = menew AnimatorFactory();
    m_storeMapper = store_mapper;
    m_needTick = false;
}

AnimatorRepository::~AnimatorRepository()
{
    SAFE_DELETE(m_factory);
}

ServiceResult AnimatorRepository::onInit()
{
    assert(m_storeMapper);

    m_queryAnimator = std::make_shared<QuerySubscriber>([=](const IQueryPtr& q) { queryAnimator(q); });
    QueryDispatcher::subscribe(typeid(QueryAnimator), m_queryAnimator);
    m_queryAnimatorNextSequenceNumber = std::make_shared<QuerySubscriber>([=](const IQueryPtr& q) { queryAnimatorNextSequenceNumber(q); });
    QueryDispatcher::subscribe(typeid(QueryAnimatorNextSequenceNumber), m_queryAnimatorNextSequenceNumber);

    m_putAnimator = std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { putAnimator(c); });
    CommandBus::subscribe(typeid(PutAnimator), m_putAnimator);
    m_removeAnimator = std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { removeAnimator(c); });
    CommandBus::subscribe(typeid(RemoveAnimator), m_removeAnimator);

    m_storeMapper->connect();
    return ServiceResult::Complete;
}

ServiceResult AnimatorRepository::onTerm()
{
    assert(m_storeMapper);
    m_storeMapper->disconnect();
    m_animators.clear();

    QueryDispatcher::unsubscribe(typeid(QueryAnimator), m_queryAnimator);
    m_queryAnimator = nullptr;
    QueryDispatcher::unsubscribe(typeid(QueryAnimatorNextSequenceNumber), m_queryAnimatorNextSequenceNumber);
    m_queryAnimatorNextSequenceNumber = nullptr;

    CommandBus::unsubscribe(typeid(PutAnimator), m_putAnimator);
    m_putAnimator = nullptr;
    CommandBus::unsubscribe(typeid(RemoveAnimator), m_removeAnimator);
    m_removeAnimator = nullptr;

    return ServiceResult::Complete;
}

std::uint64_t AnimatorRepository::nextSequenceNumber()
{
    assert(m_storeMapper);
    return m_storeMapper->nextSequenceNumber();
}

bool AnimatorRepository::hasAnimator(const AnimatorId& id)
{
    assert(m_storeMapper);
    std::lock_guard locker{ m_animatorLock };
    const auto it = m_animators.find(id);
    if (it != m_animators.end()) return true;
    return m_storeMapper->hasAnimator(id.origin());
}

std::shared_ptr<Animator> AnimatorRepository::queryAnimator(const AnimatorId& id)
{
    if (!hasAnimator(id)) return nullptr;
    std::lock_guard locker{ m_animatorLock };
    auto it = m_animators.find(id);
    if (it != m_animators.end()) return it->second;
    assert(m_factory);
    const auto dto = m_storeMapper->queryAnimator(id.origin());
    assert(dto.has_value());
    auto anim = m_factory->constitute(id, dto.value(), true);
    assert(anim);
    m_animators.insert_or_assign(id, anim);
    return anim;
}

void AnimatorRepository::removeAnimator(const AnimatorId& id)
{
    if (!hasAnimator(id)) return;
    std::lock_guard locker{ m_animatorLock };
    m_animators.erase(id);
    error er = m_storeMapper->removeAnimator(id.origin());
    if (er)
    {
        Platforms::Debug::ErrorPrintf("remove animator %s failed : %s\n", id.name().c_str(), er.message().c_str());
        EventPublisher::post(std::make_shared<RemoveAnimatorFailed>(id, er));
    }
}

void AnimatorRepository::putAnimator(const AnimatorId& id, const std::shared_ptr<Animator>& animator)
{
    assert(animator);
    std::lock_guard locker{ m_animatorLock };
    m_animators.insert_or_assign(id, animator);
    if (id != id.origin()) return;  // only put origin animator to store
    error er = m_storeMapper->putAnimator(id.origin(), animator->serializeDto());
    if (er)
    {
        Platforms::Debug::ErrorPrintf("put animator %s failed : %s\n", id.name().c_str(), er.message().c_str());
        EventPublisher::post(std::make_shared<PutAnimatorFailed>(id, er));
    }
}

void AnimatorRepository::queryAnimator(const Frameworks::IQueryPtr& q)
{
    if (!q) return;
    const auto query = std::dynamic_pointer_cast<QueryAnimator, IQuery>(q);
    if (!query) return;
    query->setResult(queryAnimator(query->id()));
}

void AnimatorRepository::queryAnimatorNextSequenceNumber(const Frameworks::IQueryPtr& q)
{
    if (!q) return;
    const auto query = std::dynamic_pointer_cast<QueryAnimatorNextSequenceNumber, IQuery>(q);
    if (!query) return;
    query->setResult(nextSequenceNumber());
}

void AnimatorRepository::putAnimator(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<PutAnimator, ICommand>(c);
    if (!cmd) return;
    putAnimator(cmd->id(), cmd->animator());
}

void AnimatorRepository::removeAnimator(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<RemoveAnimator, ICommand>(c);
    if (!cmd) return;
    removeAnimator(cmd->id());
}


