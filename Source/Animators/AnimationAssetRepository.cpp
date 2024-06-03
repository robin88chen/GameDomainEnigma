#include "AnimationAssetRepository.h"
#include "AnimationAssetStoreMapper.h"
#include "AnimationAssetFactory.h"
#include "AnimationAssetCommands.h"
#include "AnimationAssetQueries.h"
#include "AnimationAssetEvents.h"
#include "AnimatorErrors.h"
#include "Frameworks/CommandBus.h"
#include "Frameworks/QueryDispatcher.h"
#include "Frameworks/EventPublisher.h"
#include "Platforms/MemoryMacro.h"
#include "Platforms/PlatformLayer.h"
#include <cassert>

using namespace Enigma::Animators;
using namespace Enigma::Frameworks;

DEFINE_RTTI(Animators, AnimationAssetRepository, ISystemService);

using error = std::error_code;

AnimationAssetRepository::AnimationAssetRepository(ServiceManager* srv_manager, const std::shared_ptr<AnimationAssetStoreMapper>& store_mapper) : ISystemService(srv_manager)
{
    m_storeMapper = store_mapper;
    m_factory = menew AnimationAssetFactory();
}

AnimationAssetRepository::~AnimationAssetRepository()
{
    SAFE_DELETE(m_factory);
    m_animationAssets.clear();
}

ServiceResult AnimationAssetRepository::onInit()
{
    assert(m_storeMapper);

    m_queryAnimationAsset = std::make_shared<QuerySubscriber>([=](const IQueryPtr& q) { queryAnimationAsset(q); });
    QueryDispatcher::subscribe(typeid(QueryAnimationAsset), m_queryAnimationAsset);
    m_requestAnimationAssetCreation = std::make_shared<QuerySubscriber>([=](const IQueryPtr& q) { requestAnimationAssetCreation(q); });
    QueryDispatcher::subscribe(typeid(RequestAnimationAssetCreation), m_requestAnimationAssetCreation);
    m_requestAnimationAssetConstitution = std::make_shared<QuerySubscriber>([=](const IQueryPtr& q) { requestAnimationAssetConstitution(q); });
    QueryDispatcher::subscribe(typeid(RequestAnimationAssetConstitution), m_requestAnimationAssetConstitution);
    m_removeAnimationAsset = std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { removeAnimationAsset(c); });
    CommandBus::subscribe(typeid(RemoveAnimationAsset), m_removeAnimationAsset);
    m_putAnimationAsset = std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { putAnimationAsset(c); });
    CommandBus::subscribe(typeid(PutAnimationAsset), m_putAnimationAsset);

    m_storeMapper->connect();
    return ServiceResult::Complete;
}

ServiceResult AnimationAssetRepository::onTerm()
{
    assert(m_storeMapper);
    QueryDispatcher::unsubscribe(typeid(QueryAnimationAsset), m_queryAnimationAsset);
    m_queryAnimationAsset = nullptr;
    QueryDispatcher::unsubscribe(typeid(RequestAnimationAssetCreation), m_requestAnimationAssetCreation);
    m_requestAnimationAssetCreation = nullptr;
    QueryDispatcher::unsubscribe(typeid(RequestAnimationAssetConstitution), m_requestAnimationAssetConstitution);
    m_requestAnimationAssetConstitution = nullptr;
    CommandBus::unsubscribe(typeid(RemoveAnimationAsset), m_removeAnimationAsset);
    m_removeAnimationAsset = nullptr;
    CommandBus::unsubscribe(typeid(PutAnimationAsset), m_putAnimationAsset);
    m_putAnimationAsset = nullptr;

    m_storeMapper->disconnect();
    return ServiceResult::Complete;
}

void AnimationAssetRepository::registerAnimationAssetFactory(const std::string& rtti_name, const AnimationAssetCreator& creator, const AnimationAssetConstitutor& constitutor)
{
    assert(m_factory);
    m_factory->registerAnimationAssetFactory(rtti_name, creator, constitutor);
}

bool AnimationAssetRepository::hasAnimationAsset(const AnimationAssetId& id)
{
    assert(m_storeMapper);
    std::lock_guard lock(m_animationAssetLock);
    const auto& it = m_animationAssets.find(id);
    if (it != m_animationAssets.end()) return true;
    return m_storeMapper->hasAnimationAsset(id);
}

std::shared_ptr<AnimationAsset> AnimationAssetRepository::queryAnimationAsset(const AnimationAssetId& id)
{
    if (!hasAnimationAsset(id)) return nullptr;
    std::lock_guard lock(m_animationAssetLock);
    const auto& it = m_animationAssets.find(id);
    if (it != m_animationAssets.end()) return it->second;
    assert(m_factory);
    const auto dto = m_storeMapper->queryAnimationAsset(id);
    assert(dto.has_value());
    auto animation = m_factory->constitute(id, dto.value(), true);
    assert(animation);
    m_animationAssets.insert_or_assign(id, animation);
    return animation;
}

void AnimationAssetRepository::removeAnimationAsset(const AnimationAssetId& id)
{
    if (!hasAnimationAsset(id)) return;
    std::lock_guard locker{ m_animationAssetLock };
    m_animationAssets.erase(id);
    error er = m_storeMapper->removeAnimationAsset(id);
    if (er)
    {
        Platforms::Debug::ErrorPrintf("remove animation asset %s failed : %s\n", id.name().c_str(), er.message().c_str());
        EventPublisher::enqueue(std::make_shared<RemoveAnimationAssetFailed>(id, er));
    }
    else
    {
        EventPublisher::enqueue(std::make_shared<AnimationAssetRemoved>(id));
    }
}

void AnimationAssetRepository::putAnimationAsset(const AnimationAssetId& id, const std::shared_ptr<AnimationAsset>& asset)
{
    assert(asset);
    assert(m_storeMapper);
    std::lock_guard locker{ m_animationAssetLock };
    error er = m_storeMapper->putAnimationAsset(id, asset->serializeDto());
    if (er)
    {
        Platforms::Debug::ErrorPrintf("put animation asset %s failed : %s\n", id.name().c_str(), er.message().c_str());
        EventPublisher::enqueue(std::make_shared<PutAnimationAssetFailed>(id, er));
    }
    else
    {
        EventPublisher::enqueue(std::make_shared<AnimationAssetPut>(id));
    }
}

void AnimationAssetRepository::queryAnimationAsset(const IQueryPtr& q)
{
    if (!q) return;
    const auto query = std::dynamic_pointer_cast<QueryAnimationAsset>(q);
    if (!query) return;
    query->setResult(queryAnimationAsset(query->id()));
}

void AnimationAssetRepository::requestAnimationAssetCreation(const Frameworks::IQueryPtr& r)
{
    assert(m_factory);
    if (!r) return;
    auto request = std::dynamic_pointer_cast<RequestAnimationAssetCreation>(r);
    if (!request) return;
    if (hasAnimationAsset(request->id()))
    {
        EventPublisher::enqueue(std::make_shared<CreateAnimationAssetFailed>(request->id(), ErrorCode::animationAssetAlreadyExists));
        return;
    }
    auto animation = m_factory->create(request->id(), request->rtti());
    assert(animation);
    std::lock_guard lock(m_animationAssetLock);
    m_animationAssets.insert_or_assign(request->id(), animation);
    request->setResult(animation);
}

void AnimationAssetRepository::requestAnimationAssetConstitution(const Frameworks::IQueryPtr& r)
{
    if (!r) return;
    auto request = std::dynamic_pointer_cast<RequestAnimationAssetConstitution>(r);
    if (!request) return;
    if (hasAnimationAsset(request->id()))
    {
        EventPublisher::enqueue(std::make_shared<ConstituteAnimationAssetFailed>(request->id(), ErrorCode::animationAssetAlreadyExists));
        return;
    }
    auto animation = m_factory->constitute(request->id(), request->dto(), false);
    assert(animation);
    std::lock_guard lock(m_animationAssetLock);
    m_animationAssets.insert_or_assign(request->id(), animation);
    request->setResult(animation);
}

void AnimationAssetRepository::removeAnimationAsset(const ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<RemoveAnimationAsset>(c);
    if (!cmd) return;
    removeAnimationAsset(cmd->id());
}

void AnimationAssetRepository::putAnimationAsset(const ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<PutAnimationAsset>(c);
    if (!cmd) return;
    putAnimationAsset(cmd->id(), cmd->animation());
}
