#include "ModelPrimitiveAnimator.h"
#include "AnimationAssetDtos.h"
#include "Platforms/PlatformLayer.h"
#include "Renderer/ModelPrimitive.h"
#include "ModelAnimationAsset.h"
#include "SkinAnimationOperator.h"
#include <cassert>

using namespace Enigma::Animators;
using namespace Enigma::Frameworks;
using namespace Enigma::Renderer;
using namespace Enigma::Engine;

DEFINE_RTTI(Animators, ModelPrimitiveAnimator, Animator);

ModelPrimitiveAnimator::ModelPrimitiveAnimator(const AnimatorId& id) : Animator(id)
{
    m_factoryDesc = FactoryDesc(ModelPrimitiveAnimator::TYPE_RTTI.getName());
    m_animationAsset = nullptr;
    m_meshNodeMapping.clear();
    m_skinAnimOperators.clear();

    m_remainFadingTime = 0.0f;
    m_fadingTime = 0.1f;
    m_isFading = false;
    m_isOnPlay = false;
}

ModelPrimitiveAnimator::~ModelPrimitiveAnimator()
{
    m_animationAsset = nullptr;
    m_meshNodeMapping.clear();
    m_skinAnimOperators.clear();
}

GenericDto ModelPrimitiveAnimator::serializeDto() const
{
    ModelAnimatorDto dto;
    dto.id() = id();
    dto.factoryDesc() = m_factoryDesc;
    if (!m_animationAsset) return dto.toGenericDto();
    dto.AssetName() = m_animationAsset->getName();
    dto.AssetFactoryDesc() = m_animationAsset->factoryDesc();
    if ((m_animationAsset->factoryDesc().GetInstanceType() == FactoryDesc::InstanceType::Native)
        || (m_animationAsset->factoryDesc().GetInstanceType() == FactoryDesc::InstanceType::ResourceAsset))
    {
        dto.AnimationAssetDto() = m_animationAsset->serializeDto().toGenericDto();
    }
    for (auto& op : m_skinAnimOperators)
    {
        dto.SkinOperators().emplace_back(op.serializeDto().toGenericDto());
    }
    return dto.toGenericDto();
}

Animator::HasUpdated ModelPrimitiveAnimator::update(const std::unique_ptr<Timer>& timer)
{
    if (FATAL_LOG_EXPR(!timer)) return HasUpdated::False;
    if (!m_isOnPlay) return HasUpdated::False;

    const float elapse_time = timer->getElapseTime();
    auto next_to_stop = m_currentAnimClip.update(elapse_time);

    if (m_isFading)
    {
        m_fadeInAnimClip.update(elapse_time);
        m_remainFadingTime -= elapse_time;
    }

    const auto res = updateTimeValue();

    if (static_cast<bool>(next_to_stop))
    {
        m_isOnPlay = false;
    }
    return res ? HasUpdated::True : HasUpdated::False;
}

void ModelPrimitiveAnimator::reset()
{
    m_currentAnimClip.reset();
    updateTimeValue();
}

void ModelPrimitiveAnimator::setControlledModel(const std::shared_ptr<ModelPrimitive>& model)
{
    m_controlledPrimitive = model;
    if (model) model->attachAnimator(shared_from_this());
}

std::shared_ptr<ModelPrimitive> ModelPrimitiveAnimator::getControlledModel() const
{
    if (m_controlledPrimitive.expired()) return nullptr;
    return m_controlledPrimitive.lock();
}

void ModelPrimitiveAnimator::linkAnimationAsset(const std::shared_ptr<ModelAnimationAsset>& anim_asset)
{
    m_animationAsset = anim_asset;
    calculateMeshNodeMapping();
}

void ModelPrimitiveAnimator::calculateMeshNodeMapping()
{
    unsigned mesh_count = 0;
    if (m_controlledPrimitive.expired())
    {
        m_meshNodeMapping.clear();
        return;
    }
    std::shared_ptr<ModelPrimitive> model = m_controlledPrimitive.lock();
    assert(model);
    assert(model->getMeshNodeTree().getMeshNodeCount() > 0);
    if (mesh_count < model->getMeshNodeTree().getMeshNodeCount())
    {
        mesh_count = model->getMeshNodeTree().getMeshNodeCount();
    }

    if (mesh_count == 0)
    {
        m_meshNodeMapping.clear();
        return;
    }

    m_meshNodeMapping.resize(mesh_count);
    for (unsigned int m = 0; m < mesh_count; m++)
    {
        m_meshNodeMapping[m].m_nodeIndexInModel = m;
        auto mesh_node = model->getMeshNodeTree().getMeshNode(m);
        if ((m_animationAsset) && (mesh_node))
        {
            m_meshNodeMapping[m].m_nodeIndexInAnimation =
                m_animationAsset->findMeshNodeIndex(mesh_node.value().get().getName());
        }
    }
}

void ModelPrimitiveAnimator::linkSkinMesh(const std::shared_ptr<SkinMeshPrimitive>& skin_prim,
    const std::vector<std::string>& boneNodeNames)
{
    SkinAnimationOperator skin_oper = SkinAnimationOperator();
    skin_oper.linkSkinMeshPrimitive(skin_prim, boneNodeNames);
    if (!m_controlledPrimitive.expired())
    {
        std::shared_ptr<ModelPrimitive> model = m_controlledPrimitive.lock();
        skin_oper.calculateNodeOffsetMatrix(model, skin_prim->getOwnerRootRefTransform());
    }
    m_skinAnimOperators.emplace_back(skin_oper);
}

void ModelPrimitiveAnimator::linkSkinMesh(const std::shared_ptr<SkinMeshPrimitive>& skin_prim,
    const std::vector<std::string>& boneNodeNames, const std::vector<MathLib::Matrix4>& boneNodeOffsets)
{
    SkinAnimationOperator skin_oper = SkinAnimationOperator();
    skin_oper.linkSkinMeshPrimitive(skin_prim, boneNodeNames);
    if (!m_controlledPrimitive.expired())
    {
        std::shared_ptr<ModelPrimitive> model = m_controlledPrimitive.lock();
        skin_oper.linkNodeOffsetMatrix(model, boneNodeOffsets);
    }
    m_skinAnimOperators.emplace_back(skin_oper);
}

const SkinAnimationOperator& ModelPrimitiveAnimator::getSkinAnimOperator(unsigned index)
{
    assert(index < m_skinAnimOperators.size());
    return m_skinAnimOperators[index];
}

void ModelPrimitiveAnimator::playAnimation(const AnimationClip& clip)
{
    m_currentAnimClip = clip;
    m_isOnPlay = true;
}

void ModelPrimitiveAnimator::fadeInAnimation(float fading_time, const AnimationClip& clip)
{
    if (!m_isOnPlay)
    {
        playAnimation(clip);
    }
    else
    {
        if (m_currentAnimClip.remainLoopTime() < fading_time)  // 剩下的時間不夠做 fading
        {
            fading_time = m_currentAnimClip.remainLoopTime();
        }
        m_fadeInAnimClip = clip;
        m_fadingTime = fading_time;
        m_remainFadingTime = fading_time;
        m_isFading = true;
    }
}

void ModelPrimitiveAnimator::stopAnimation()
{
    m_isOnPlay = false;
}

bool ModelPrimitiveAnimator::updateTimeValue()
{
    if (m_isFading)
    {
        const bool hasUpdate = updateMeshNodeTransformWithFading();
        if (!hasUpdate) return false;
    }
    else
    {  // none fading animation
        const bool hasUpdate = updateMeshNodeTransform();
        if (!hasUpdate) return false;
    }
    if (m_controlledPrimitive.expired()) return false;
    std::shared_ptr<ModelPrimitive> model = m_controlledPrimitive.lock();
    if (!model) return false;
    if (m_skinAnimOperators.size() == 1)
    {
        m_skinAnimOperators[0].updateSkinMeshBoneMatrix(model->getMeshNodeTree());
    }
    else
    {
        for (auto& op : m_skinAnimOperators)
        {
            op.updateSkinMeshBoneMatrix(model->getMeshNodeTree());
        }
    }

    return true;
}

bool ModelPrimitiveAnimator::updateMeshNodeTransformWithFading()
{
    if (m_meshNodeMapping.empty()) return false;
    if (m_controlledPrimitive.expired()) return false;
    const std::shared_ptr<ModelPrimitive> model = m_controlledPrimitive.lock();
    if (!model) return false;
    if (model->getMeshNodeTree().getMeshNodeCount() == 0) return false;
    if (!m_animationAsset) return false;

    const float current_time_value = m_currentAnimClip.currentTimeValue();
    float fading_weight = m_remainFadingTime / m_fadingTime;
    if (m_remainFadingTime <= 0.0f) fading_weight = 0.0f;

    float fadein_time_value = m_fadeInAnimClip.currentTimeValue();
    const unsigned mesh_count = model->getMeshNodeTree().getMeshNodeCount();
    for (unsigned m = 0; m < mesh_count; m++)
    {
        auto mesh_index = m_meshNodeMapping[m].m_nodeIndexInModel;
        if (!mesh_index) continue;
        auto mesh_node = model->getMeshNodeTree().getMeshNode(mesh_index.value());
        if (!mesh_node) continue;
        if (auto ani_index = m_meshNodeMapping[m].m_nodeIndexInAnimation)
        {
            model->updateMeshNodeLocalTransform(mesh_index.value(),
                m_animationAsset->calculateFadedTransformMatrix
                (ani_index.value(), current_time_value, fadein_time_value, fading_weight));
        }
        else
        {
            // 沒有這個node 的 animation, 用 mesh node 的原始local transform 更新
            model->updateMeshNodeLocalTransform(mesh_index.value(), mesh_node.value().get().getLocalTransform());
        }
    }

    if (m_remainFadingTime <= 0.0f)
    { // clear fading state
        m_isFading = false;
        m_currentAnimClip = m_fadeInAnimClip;
    }
    return true;
}

bool ModelPrimitiveAnimator::updateMeshNodeTransform()
{
    if (m_meshNodeMapping.empty()) return false;
    if (m_controlledPrimitive.expired()) return false;
    const std::shared_ptr<ModelPrimitive> model = m_controlledPrimitive.lock();
    if (!model) return false;
    if (model->getMeshNodeTree().getMeshNodeCount() == 0) return false;
    if (!m_animationAsset) return false;

    const float current_time_value = m_currentAnimClip.currentTimeValue();
    const unsigned mesh_count = model->getMeshNodeTree().getMeshNodeCount();
    for (unsigned m = 0; m < mesh_count; m++)
    {
        auto mesh_index = m_meshNodeMapping[m].m_nodeIndexInModel;
        if (!mesh_index) continue;
        auto mesh_node = model->getMeshNodeTree().getMeshNode(mesh_index.value());
        if (!mesh_node) continue;
        if (auto ani_index = m_meshNodeMapping[m].m_nodeIndexInAnimation)
        {
            model->updateMeshNodeLocalTransform(mesh_index.value(),
                m_animationAsset->calculateTransformMatrix(ani_index.value(), current_time_value));
        }
        else
        {
            // 沒有這個node 的 animation, 用 mesh node 的原始local transform 更新
            model->updateMeshNodeLocalTransform(mesh_index.value(), mesh_node.value().get().getLocalTransform());
        }
    }
    return true;
}
