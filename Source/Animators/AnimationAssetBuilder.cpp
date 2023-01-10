#include "AnimationAssetBuilder.h"
#include "AnimationAssetPolicies.h"
#include "AnimationRepository.h"
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
    : m_ruidConstructing(Ruid::Generate()), m_ruidDeserializing(Ruid::Generate())
{
    m_repository = host;
    m_onDtoAnimationCreated = std::make_shared<EventSubscriber>([=](auto e) { this->OnDtoAnimationAssetCreated(e); });
    m_onDtoDeserialized = std::make_shared<EventSubscriber>([=](auto e) { this->OnDtoDeserialized(e); });
    m_onDeserializeDtoFailed = std::make_shared<EventSubscriber>([=](auto e) { this->OnDeserializeDtoFailed(e); });

    EventPublisher::Subscribe(typeid(FactoryAnimationAssetCreated), m_onDtoAnimationCreated);
    EventPublisher::Subscribe(typeid(GenericDtoDeserialized), m_onDtoDeserialized);
    EventPublisher::Subscribe(typeid(DeserializeDtoFailed), m_onDeserializeDtoFailed);
}

AnimationAssetBuilder::~AnimationAssetBuilder()
{
    EventPublisher::Unsubscribe(typeid(FactoryAnimationAssetCreated), m_onDtoAnimationCreated);
    EventPublisher::Unsubscribe(typeid(GenericDtoDeserialized), m_onDtoDeserialized);
    EventPublisher::Unsubscribe(typeid(DeserializeDtoFailed), m_onDeserializeDtoFailed);

    m_onDtoAnimationCreated = nullptr;
    m_onDtoDeserialized = nullptr;
    m_onDeserializeDtoFailed = nullptr;
}

void AnimationAssetBuilder::BuildAnimationAsset(const std::shared_ptr<AnimationAssetPolicy>& policy)
{
    assert(m_repository);
    m_policy = policy;
    if (m_repository->HasAnimationAsset(policy->RttiName(), policy->Name()))
    {
        EventPublisher::Post(std::make_shared<AnimationAssetBuilt>(policy->Name(),
            m_repository->QueryAnimationAsset(policy->RttiName(), policy->Name())));
    }
    else if (policy->GetDto())
    {
        CreateFromDto(policy->Name(), policy->GetDto().value());
    }
    else if (policy->GetDeserializer())
    {
        m_ruidDeserializing = Ruid::Generate();
        policy->GetDeserializer()->InvokeDeserialize(m_ruidDeserializing, policy->Parameter());
    }
    else
    {
        EventPublisher::Post(std::make_shared<BuildAnimationAssetFailed>(policy->Name(), ErrorCode::policyIncomplete));
    }
}

void AnimationAssetBuilder::CreateFromDto(const std::string& name, const GenericDto& dto)
{
    assert(m_repository);
    m_ruidConstructing = dto.GetId();
    CommandBus::Post(std::make_shared<InvokeDtoFactory>(dto));
}

void AnimationAssetBuilder::OnDtoAnimationAssetCreated(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<FactoryAnimationAssetCreated, IEvent>(e);
    if (!ev) return;
    if (ev->GetConstructingRuid() != m_ruidConstructing) return;
    EventPublisher::Post(std::make_shared<AnimationAssetBuilt>(m_policy->Name(), ev->GetAnimationAsset()));
}

void AnimationAssetBuilder::OnDtoDeserialized(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<GenericDtoDeserialized, IEvent>(e);
    if (!ev) return;
    if (ev->GetRuid() != m_ruidDeserializing) return;
    if (ev->GetDtos().empty())
    {
        EventPublisher::Post(std::make_shared<BuildAnimationAssetFailed>(m_policy->Name(), ErrorCode::deserializeFail));
        return;
    }
    CreateFromDto(m_policy->Name(), ev->GetDtos()[0]);
}

void AnimationAssetBuilder::OnDeserializeDtoFailed(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<DeserializeDtoFailed, IEvent>(e);
    if (!ev) return;
    if (ev->GetRuid() != m_ruidDeserializing) return;
    EventPublisher::Post(std::make_shared<BuildAnimationAssetFailed>(m_policy->Name(), ev->GetErrorCode()));
}
