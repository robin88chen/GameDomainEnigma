#include "ModelAnimatorBuilder.h"
#include "AnimationRepository.h"
#include "AnimatorPolicies.h"
#include "AnimationAssetPolicies.h"
#include "ModelPrimitiveAnimator.h"
#include "Frameworks/CommandBus.h"
#include "AnimationAssetCommand.h"
#include "Frameworks/EventPublisher.h"
#include "AnimatorErrors.h"
#include "AnimatorEvents.h"
#include "AnimationAssetEvents.h"
#include "ModelAnimationAsset.h"
#include <cassert>

using namespace Enigma::Animators;
using namespace Enigma::Frameworks;

ModelAnimatorBuilder::ModelAnimatorBuilder(AnimationRepository* host)
{
    m_repository = host;
    m_builtAnimator = nullptr;

    m_onAnimationAssetBuilt = std::make_shared<EventSubscriber>([=](auto e) { this->OnAnimationAssetBuilt(e); });
    m_onBuildAnimationAssetFailed = std::make_shared<EventSubscriber>([=](auto e) { this->OnBuildAnimationAssetFailed(e); });

    EventPublisher::Subscribe(typeid(AnimationAssetBuilt), m_onAnimationAssetBuilt);
    EventPublisher::Subscribe(typeid(BuildAnimationAssetFailed), m_onBuildAnimationAssetFailed);
}

ModelAnimatorBuilder::~ModelAnimatorBuilder()
{
    EventPublisher::Unsubscribe(typeid(AnimationAssetBuilt), m_onAnimationAssetBuilt);
    EventPublisher::Unsubscribe(typeid(BuildAnimationAssetFailed), m_onBuildAnimationAssetFailed);

    m_onAnimationAssetBuilt = nullptr;
    m_onBuildAnimationAssetFailed = nullptr;

    m_builtAnimator = nullptr;
}

void ModelAnimatorBuilder::BuildModelAnimator(const std::shared_ptr<ModelAnimatorPolicy>& policy)
{
    assert(m_repository);
    m_policy = policy;
    m_assetName = "";
    m_builtAnimator = std::make_shared<ModelPrimitiveAnimator>();
    m_builtAnimator->SetControlledModel(m_policy->ControlledPrimitive());
    if (m_policy->GetAssetPolicy())
    {
        m_assetName = m_policy->GetAssetPolicy()->Name();
        CommandBus::Post(std::make_shared<BuildAnimationAsset>(m_policy->GetAssetPolicy()));
    }
    else
    {
        EventPublisher::Post(std::make_shared<BuildModelAnimatorFailed>(m_policy->GetRuid(), ErrorCode::policyIncomplete));
    }
}

void ModelAnimatorBuilder::OnAnimationAssetBuilt(const Frameworks::IEventPtr& e)
{
    assert(m_builtAnimator);
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<AnimationAssetBuilt, IEvent>(e);
    if (!ev) return;
    if (ev->GetName() != m_assetName) return;
    auto model_anim = std::dynamic_pointer_cast<ModelAnimationAsset, AnimationAsset>(ev->GetAnimationAsset());
    if (!model_anim)
    {
        EventPublisher::Post(std::make_shared<BuildModelAnimatorFailed>(m_policy->GetRuid(), ErrorCode::dynamicCastFail));
        return;
    }
    m_builtAnimator->LinkAnimationAsset(model_anim);
    EventPublisher::Post(std::make_shared<ModelAnimatorBuilt>(m_policy->GetRuid(), m_builtAnimator));
}

void ModelAnimatorBuilder::OnBuildAnimationAssetFailed(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<BuildAnimationAssetFailed, IEvent>(e);
    if (!ev) return;
    if (ev->GetName() != m_assetName) return;
    EventPublisher::Post(std::make_shared<BuildModelAnimatorFailed>(m_policy->GetRuid(), ev->GetErrorCode()));
}
