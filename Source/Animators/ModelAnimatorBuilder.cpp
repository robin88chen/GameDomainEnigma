#include "ModelAnimatorBuilder.h"
#include "ModelAnimationRepository.h"
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

ModelAnimatorBuilder::ModelAnimatorBuilder(AnimationRepository* host) : m_originalAssetDesc(Engine::AnimationAsset::TYPE_RTTI.getName())
{
    m_repository = host;
    m_builtAnimator = nullptr;

    m_onAnimationAssetBuilt = std::make_shared<EventSubscriber>([=](auto e) { this->OnAnimationAssetBuilt(e); });
    m_onBuildAnimationAssetFailed = std::make_shared<EventSubscriber>([=](auto e) { this->OnBuildAnimationAssetFailed(e); });

    EventPublisher::subscribe(typeid(AnimationAssetBuilt), m_onAnimationAssetBuilt);
    EventPublisher::subscribe(typeid(BuildAnimationAssetFailed), m_onBuildAnimationAssetFailed);
}

ModelAnimatorBuilder::~ModelAnimatorBuilder()
{
    EventPublisher::unsubscribe(typeid(AnimationAssetBuilt), m_onAnimationAssetBuilt);
    EventPublisher::unsubscribe(typeid(BuildAnimationAssetFailed), m_onBuildAnimationAssetFailed);

    m_onAnimationAssetBuilt = nullptr;
    m_onBuildAnimationAssetFailed = nullptr;

    m_builtAnimator = nullptr;
}

void ModelAnimatorBuilder::BuildModelAnimator(const std::shared_ptr<ModelAnimatorPolicy>& policy)
{
    //todo: 先收起來
    /*assert(m_repository);
    m_policy = policy;
    m_assetName = "";
    m_builtAnimator = std::make_shared<ModelPrimitiveAnimator>();
    m_builtAnimator->setControlledModel(m_policy->ControlledPrimitive());
    m_originalAssetDesc = policy->AssetFactoryDesc();
    if (m_policy->GetAssetPolicy())
    {
        m_assetName = m_policy->GetAssetPolicy()->name();
        CommandBus::post(std::make_shared<BuildAnimationAsset>(m_policy->GetAssetPolicy()));
    }
    else
    {
        EventPublisher::post(std::make_shared<BuildModelAnimatorFailed>(m_policy->getRuid(), ErrorCode::policyIncomplete));
    }*/
}

void ModelAnimatorBuilder::LinkSkinMeshOperators()
{
    if (!m_builtAnimator->getControlledModel()) return;

    for (auto& op : m_policy->SkinOperators())
    {
        if ((!op.SkinMeshName()) && (!op.SkinMeshNodeName())) continue;
        std::shared_ptr<MeshPrimitive> mesh = nullptr;
        if (auto mesh_name = op.SkinMeshName())
        {
            mesh = m_builtAnimator->getControlledModel()->findMeshPrimitive(mesh_name.value());
        }
        else if (auto node_name = op.SkinMeshNodeName())
        {
            auto& node_tree = m_builtAnimator->getControlledModel()->getMeshNodeTree();
            auto node_idx = node_tree.findMeshNodeIndex(node_name.value());
            if (node_idx) mesh = node_tree.getMeshPrimitiveInNode(node_idx.value());
        }
        if (!mesh) continue;
        auto skin_mesh = std::dynamic_pointer_cast<SkinMeshPrimitive, MeshPrimitive>(mesh);
        if (!skin_mesh) continue;
        if (auto offsets = op.NodeOffsets())
        {
            m_builtAnimator->linkSkinMesh(skin_mesh, op.BoneNodeNames(), offsets.value());
        }
        else
        {
            m_builtAnimator->linkSkinMesh(skin_mesh, op.BoneNodeNames());
        }
    }
}

void ModelAnimatorBuilder::OnAnimationAssetBuilt(const IEventPtr& e)
{
    assert(m_builtAnimator);
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<AnimationAssetBuilt, IEvent>(e);
    if (!ev) return;
    if (ev->getName() != m_assetName) return;
    auto model_anim = std::dynamic_pointer_cast<ModelAnimationAsset, Engine::AnimationAsset>(ev->GetAnimationAsset());
    if (!model_anim)
    {
        EventPublisher::post(std::make_shared<BuildModelAnimatorFailed>(m_policy->getRuid(), ErrorCode::dynamicCastFail));
        return;
    }
    model_anim->factoryDesc() = m_originalAssetDesc;
    m_builtAnimator->linkAnimationAsset(model_anim);
    if (!m_policy->SkinOperators().empty()) LinkSkinMeshOperators();
    EventPublisher::post(std::make_shared<ModelAnimatorBuilt>(m_policy->getRuid(), m_builtAnimator));
}

void ModelAnimatorBuilder::OnBuildAnimationAssetFailed(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<BuildAnimationAssetFailed, IEvent>(e);
    if (!ev) return;
    if (ev->getName() != m_assetName) return;
    EventPublisher::post(std::make_shared<BuildModelAnimatorFailed>(m_policy->getRuid(), ev->GetErrorCode()));
}
