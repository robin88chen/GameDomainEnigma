#include "AnimatorFactory.h"
#include "AnimatorCommands.h"
#include "Frameworks/CommandBus.h"
#include "Frameworks/EventPublisher.h"
#include "AnimatorEvents.h"
#include "AnimatorQueries.h"
#include "Platforms/PlatformLayer.h"
#include "AnimatorErrors.h"

using namespace Enigma::Animators;
using namespace Enigma::Frameworks;


AnimatorFactory::AnimatorFactory()
{
    registerHandlers();
}

AnimatorFactory::~AnimatorFactory()
{
    unregisterHandlers();
}

void AnimatorFactory::registerHandlers()
{
    m_registerAnimatorFactory = std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { registerAnimatorFactory(c); });
    CommandBus::subscribe(typeid(RegisterAnimatorFactory), m_registerAnimatorFactory);
    m_unregisterAnimatorFactory = std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { unregisterAnimatorFactory(c); });
    CommandBus::subscribe(typeid(UnregisterAnimatorFactory), m_unregisterAnimatorFactory);
}

void AnimatorFactory::unregisterHandlers()
{
    CommandBus::unsubscribe(typeid(RegisterAnimatorFactory), m_registerAnimatorFactory);
    m_registerAnimatorFactory = nullptr;
    CommandBus::unsubscribe(typeid(UnregisterAnimatorFactory), m_unregisterAnimatorFactory);
    m_unregisterAnimatorFactory = nullptr;
}

std::shared_ptr<Animator> AnimatorFactory::create(const AnimatorId& id, const Frameworks::Rtti& rtti)
{
    auto creator = m_creators.find(rtti.getName());
    if (creator == m_creators.end())
    {
        Platforms::Debug::Printf("Can't find creator of %s\n", rtti.getName().c_str());
        EventPublisher::enqueue(std::make_shared<CreateAnimatorFailed>(id, ErrorCode::animatorFactoryNotExists));
        return nullptr;
    }
    auto anim = creator->second(id);
    EventPublisher::enqueue(std::make_shared<AnimatorCreated>(id, anim));
    return anim;
}

std::shared_ptr<Animator> AnimatorFactory::constitute(const AnimatorId& id, const Engine::GenericDto& dto, bool is_persisted)
{
    auto constitutor = m_constitutors.find(dto.getRtti().rttiName());
    if (constitutor == m_constitutors.end())
    {
        Platforms::Debug::Printf("Can't find constitutor of %s\n", dto.getRtti().rttiName().c_str());
        EventPublisher::enqueue(std::make_shared<ConstituteAnimatorFailed>(id, ErrorCode::animatorFactoryNotExists));
        return nullptr;
    }
    auto anim = constitutor->second(id, dto);
    EventPublisher::enqueue(std::make_shared<AnimatorConstituted>(id, anim, is_persisted));
    return anim;
}

void AnimatorFactory::registerAnimatorFactory(const std::string& rtti, const AnimatorCreator& creator, const AnimatorConstitutor& constitutor)
{
    if (m_creators.find(rtti) != m_creators.end())
    {
        Platforms::Debug::Printf("Animator factory of %s already exists\n", rtti.c_str());
        return;
    }
    m_creators[rtti] = creator;
    m_constitutors[rtti] = constitutor;
}

void AnimatorFactory::unregisterAnimatorFactory(const std::string& rtti)
{
    if (m_creators.find(rtti) == m_creators.end())
    {
        Platforms::Debug::Printf("Animator factory of %s doesn't exist\n", rtti.c_str());
        return;
    }
    m_creators.erase(rtti);
    m_constitutors.erase(rtti);
}

void AnimatorFactory::registerAnimatorFactory(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<RegisterAnimatorFactory>(c);
    if (!cmd) return;
    registerAnimatorFactory(cmd->rttiName(), cmd->creator(), cmd->constitutor());
}

void AnimatorFactory::unregisterAnimatorFactory(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<UnregisterAnimatorFactory>(c);
    if (!cmd) return;
    unregisterAnimatorFactory(cmd->rttiName());
}
