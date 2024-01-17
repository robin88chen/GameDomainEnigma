#include "AnimationAssetBuilder.h"
#include "AnimationAssetPolicies.h"
#include "ModelAnimationRepository.h"
#include "Frameworks/EventPublisher.h"
#include "AnimationAssetEvents.h"
#include "Frameworks/CommandBus.h"
#include "GameEngine/DtoEvents.h"
#include "GameEngine/FactoryCommands.h"
#include "AnimatorErrors.h"
#include <memory>

using namespace Enigma::Animators;
using namespace Enigma::Frameworks;
using namespace Enigma::Engine;

AnimationAssetBuilder::AnimationAssetBuilder(AnimationRepository* host)
    : m_ruidConstructing(Ruid::generate()), m_ruidDeserializing(Ruid::generate())
{
    m_repository = host;
    m_onDtoAnimationCreated = std::make_shared<EventSubscriber>([=](auto e) { this->OnDtoAnimationAssetCreated(e); });
    m_onDtoDeserialized = std::make_shared<EventSubscriber>([=](auto e) { this->OnDtoDeserialized(e); });
    m_onDeserializeDtoFailed = std::make_shared<EventSubscriber>([=](auto e) { this->OnDeserializeDtoFailed(e); });

    EventPublisher::subscribe(typeid(FactoryAnimationAssetCreated), m_onDtoAnimationCreated);
    EventPublisher::subscribe(typeid(GenericDtoDeserialized), m_onDtoDeserialized);
    EventPublisher::subscribe(typeid(DeserializeDtoFailed), m_onDeserializeDtoFailed);
}

AnimationAssetBuilder::~AnimationAssetBuilder()
{
    EventPublisher::unsubscribe(typeid(FactoryAnimationAssetCreated), m_onDtoAnimationCreated);
    EventPublisher::unsubscribe(typeid(GenericDtoDeserialized), m_onDtoDeserialized);
    EventPublisher::unsubscribe(typeid(DeserializeDtoFailed), m_onDeserializeDtoFailed);

    m_onDtoAnimationCreated = nullptr;
    m_onDtoDeserialized = nullptr;
    m_onDeserializeDtoFailed = nullptr;
}

void AnimationAssetBuilder::BuildAnimationAsset(const std::shared_ptr<AnimationAssetPolicy>& policy)
{
    assert(m_repository);
    m_policy = policy;
    if (m_repository->HasAnimationAsset(policy->RttiName(), policy->name()))
    {
        EventPublisher::post(std::make_shared<AnimationAssetBuilt>(policy->name(),
            m_repository->QueryAnimationAsset(policy->RttiName(), policy->name())));
    }
    else if (auto dto = policy->GetDto())
    {
        CreateFromDto(policy->name(), dto.value());
    }
    else if (policy->GetDeserializer())
    {
        m_ruidDeserializing = Ruid::generate();
        policy->GetDeserializer()->invokeDeserialize(m_ruidDeserializing, policy->Parameter());
    }
    else
    {
        EventPublisher::post(std::make_shared<BuildAnimationAssetFailed>(policy->name(), ErrorCode::policyIncomplete));
    }
}

void AnimationAssetBuilder::CreateFromDto(const std::string& name, const GenericDto& dto)
{
    assert(m_repository);
    m_ruidConstructing = dto.ruid();
    CommandBus::post(std::make_shared<InvokeDtoFactory>(dto));
}

void AnimationAssetBuilder::OnDtoAnimationAssetCreated(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<FactoryAnimationAssetCreated, IEvent>(e);
    if (!ev) return;
    if (ev->GetConstructingRuid() != m_ruidConstructing) return;
    EventPublisher::post(std::make_shared<AnimationAssetBuilt>(m_policy->name(), ev->GetAnimationAsset()));
}

void AnimationAssetBuilder::OnDtoDeserialized(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<GenericDtoDeserialized, IEvent>(e);
    if (!ev) return;
    if (ev->getRuid() != m_ruidDeserializing) return;
    if (ev->GetDtos().empty())
    {
        EventPublisher::post(std::make_shared<BuildAnimationAssetFailed>(m_policy->name(), ErrorCode::deserializeFail));
        return;
    }
    CreateFromDto(m_policy->name(), ev->GetDtos()[0]);
}

void AnimationAssetBuilder::OnDeserializeDtoFailed(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<DeserializeDtoFailed, IEvent>(e);
    if (!ev) return;
    if (ev->getRuid() != m_ruidDeserializing) return;
    EventPublisher::post(std::make_shared<BuildAnimationAssetFailed>(m_policy->name(), ev->GetErrorCode()));
}
