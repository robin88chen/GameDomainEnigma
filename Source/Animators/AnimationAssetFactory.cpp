#include "AnimationAssetFactory.h"
#include "Frameworks/CommandBus.h"
#include "AnimationAssetCommands.h"
#include "AnimationAssetEvents.h"
#include "Frameworks/EventPublisher.h"
#include "Platforms/PlatformLayer.h"
#include "AnimatorErrors.h"

using namespace Enigma::Animators;
using namespace Enigma::Frameworks;

AnimationAssetFactory::AnimationAssetFactory()
{
    registerHandlers();
}

AnimationAssetFactory::~AnimationAssetFactory()
{
    unregisterHandlers();
}

void AnimationAssetFactory::registerHandlers()
{
    m_registerAnimationAssetFactory = std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { registerAnimationAssetFactory(c); });
    CommandBus::subscribe(typeid(RegisterAnimationAssetFactory), m_registerAnimationAssetFactory);
    m_unregisterAnimationAssetFactory = std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { unregisterAnimationAssetFactory(c); });
    CommandBus::subscribe(typeid(UnregisterAnimationAssetFactory), m_unregisterAnimationAssetFactory);
}

void AnimationAssetFactory::unregisterHandlers()
{
    CommandBus::unsubscribe(typeid(RegisterAnimationAssetFactory), m_registerAnimationAssetFactory);
    m_registerAnimationAssetFactory = nullptr;
    CommandBus::unsubscribe(typeid(UnregisterAnimationAssetFactory), m_unregisterAnimationAssetFactory);
    m_unregisterAnimationAssetFactory = nullptr;
}

std::shared_ptr<AnimationAsset> AnimationAssetFactory::create(const AnimationAssetId& id, const Frameworks::Rtti& rtti)
{
    auto creator = m_creators.find(rtti.getName());
    if (creator == m_creators.end())
    {
        Platforms::Debug::Printf("Can't find creator of %s\n", rtti.getName().c_str());
        EventPublisher::enqueue(std::make_shared<CreateAnimationAssetFailed>(id, ErrorCode::animationFactoryNotExists));
        return nullptr;
    }
    auto anim = creator->second(id);
    EventPublisher::enqueue(std::make_shared<AnimationAssetCreated>(id, anim));
    return anim;
}

std::shared_ptr<AnimationAsset> AnimationAssetFactory::constitute(const AnimationAssetId& id, const Engine::GenericDto& dto, bool is_persisted)
{
    auto constitutor = m_constitutors.find(dto.getRtti().GetRttiName());
    if (constitutor == m_constitutors.end())
    {
        Platforms::Debug::Printf("Can't find constitutor of %s\n", dto.getRtti().GetRttiName().c_str());
        EventPublisher::enqueue(std::make_shared<ConstituteAnimationAssetFailed>(id, ErrorCode::animationFactoryNotExists));
        return nullptr;
    }
    auto animation = constitutor->second(id, dto);
    EventPublisher::enqueue(std::make_shared<AnimationAssetConstituted>(id, animation, is_persisted));
    return animation;
}

void AnimationAssetFactory::registerAnimationAssetFactory(const std::string& rtti_name, const AnimationAssetCreator& creator, const AnimationAssetConstitutor& constitutor)
{
    if (m_creators.find(rtti_name) != m_creators.end())
    {
        Platforms::Debug::Printf("Animation asset factory of %s already exists\n", rtti_name.c_str());
        return;
    }
    m_creators[rtti_name] = creator;
    m_constitutors[rtti_name] = constitutor;
}

void AnimationAssetFactory::unregisterAnimationAssetFactory(const std::string& rtti_name)
{
    if (m_creators.find(rtti_name) == m_creators.end())
    {
        Platforms::Debug::Printf("Animation asset factory of %s doesn't exist\n", rtti_name.c_str());
        return;
    }
    m_creators.erase(rtti_name);
    m_constitutors.erase(rtti_name);
}

void AnimationAssetFactory::registerAnimationAssetFactory(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::static_pointer_cast<RegisterAnimationAssetFactory>(c);
    if (!cmd) return;
    registerAnimationAssetFactory(cmd->rttiName(), cmd->creator(), cmd->constitutor());
}

void AnimationAssetFactory::unregisterAnimationAssetFactory(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::static_pointer_cast<UnregisterAnimationAssetFactory>(c);
    if (!cmd) return;
    unregisterAnimationAssetFactory(cmd->rttiName());
}

