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
using namespace Enigma::Renderer;

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

void ModelAnimatorBuilder::LinkSkinMeshOperators()
{
    if (!m_builtAnimator->GetControlledModel()) return;

    for (auto& op : m_policy->SkinOperators())
    {
        if (!op.SkinMeshName()) continue;
        auto mesh = m_builtAnimator->GetControlledModel()->FindMeshPrimitive(op.SkinMeshName().value());
        if (!mesh) continue;
        auto skin_mesh = std::dynamic_pointer_cast<SkinMeshPrimitive, MeshPrimitive>(mesh);
        if (!skin_mesh) continue;
        if (op.NodeOffsets())
        {
            m_builtAnimator->LinkSkinMesh(skin_mesh, op.BoneNodeNames(), op.NodeOffsets().value());
        }
        else
        {
            m_builtAnimator->LinkSkinMesh(skin_mesh, op.BoneNodeNames());
        }
    }
}

void ModelAnimatorBuilder::OnAnimationAssetBuilt(const IEventPtr& e)
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
    if (!m_policy->SkinOperators().empty()) LinkSkinMeshOperators();
    EventPublisher::Post(std::make_shared<ModelAnimatorBuilt>(m_policy->GetRuid(), m_builtAnimator));
}

void ModelAnimatorBuilder::OnBuildAnimationAssetFailed(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<BuildAnimationAssetFailed, IEvent>(e);
    if (!ev) return;
    if (ev->GetName() != m_assetName) return;
    EventPublisher::Post(std::make_shared<BuildModelAnimatorFailed>(m_policy->GetRuid(), ev->GetErrorCode()));
}
