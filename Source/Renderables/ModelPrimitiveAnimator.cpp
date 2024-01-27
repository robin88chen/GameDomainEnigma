#include "ModelPrimitiveAnimator.h"
#include "ModelAnimationDtos.h"
#include "ModelAnimatorDtos.h"
#include "Platforms/PlatformLayer.h"
#include "Renderables/ModelPrimitive.h"
#include "ModelAnimationAsset.h"
#include "SkinAnimationOperator.h"
#include "ModelAnimatorDtos.h"
#include <cassert>

using namespace Enigma::Renderables;
using namespace Enigma::Frameworks;
using namespace Enigma::Renderables;
using namespace Enigma::Engine;
using namespace Enigma::Animators;

DEFINE_RTTI(Renderables, ModelPrimitiveAnimator, Animator);

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

ModelPrimitiveAnimator::ModelPrimitiveAnimator(const Animators::AnimatorId& id, const Engine::GenericDto& dto) : Animator(id)
{
    ModelAnimatorDto model_ani_dto(dto);
    m_factoryDesc = model_ani_dto.factoryDesc();
    m_animationAsset = nullptr;
    m_meshNodeMapping.clear();
    m_skinAnimOperators.clear();
    m_remainFadingTime = 0.0f;
    m_fadingTime = 0.1f;
    m_isFading = false;
    m_isOnPlay = false;
    if (model_ani_dto.controlledPrimitiveId())
    {
        m_controlledPrimitiveId = model_ani_dto.controlledPrimitiveId().value();
    }
    if (model_ani_dto.animationAssetId())
    {
        m_animationAsset = std::dynamic_pointer_cast<ModelAnimationAsset>(ModelAnimationAsset::queryAnimationAsset(model_ani_dto.animationAssetId().value()));
    }
    else
    {
        m_animationAsset = nullptr;
    }
    if ((!model_ani_dto.skinOperators().empty()) && (m_controlledPrimitiveId))
    {
        for (auto& op_dto : model_ani_dto.skinOperators())
        {
            m_skinAnimOperators.emplace_back(op_dto);
        }
    }
}

ModelPrimitiveAnimator::~ModelPrimitiveAnimator()
{
    m_animationAsset = nullptr;
    m_meshNodeMapping.clear();
    m_skinAnimOperators.clear();
}

std::shared_ptr<Animator> ModelPrimitiveAnimator::create(const Animators::AnimatorId& id)
{
    return std::make_shared<ModelPrimitiveAnimator>(id);
}

std::shared_ptr<Animator> ModelPrimitiveAnimator::constitute(const Animators::AnimatorId& id, const Engine::GenericDto& dto)
{
    return std::make_shared<ModelPrimitiveAnimator>(id, dto);
}

GenericDto ModelPrimitiveAnimator::serializeDto() const
{
    ModelAnimatorDto dto;
    dto.id() = id();
    dto.factoryDesc() = m_factoryDesc;
    if (m_controlledPrimitiveId) dto.controlledPrimitiveId() = m_controlledPrimitiveId.value();
    if (!m_animationAsset) return dto.toGenericDto();
    dto.animationAssetId() = m_animationAsset->id();
    for (auto& op : m_skinAnimOperators)
    {
        dto.skinOperators().emplace_back(op.serializeDto());
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

void ModelPrimitiveAnimator::onAttachingMeshNodeTree(const Primitives::PrimitiveId& model_id, const MeshNodeTree& mesh_node_tree)
{
    m_controlledPrimitiveId = model_id;
    calculateMeshNodeMapping(mesh_node_tree);
    for (auto& op : m_skinAnimOperators)
    {
        op.onAttachingMeshNodeTree(mesh_node_tree);
    }
}

void ModelPrimitiveAnimator::calculateMeshNodeMapping(const MeshNodeTree& mesh_node_tree)
{
    const unsigned mesh_count = mesh_node_tree.getMeshNodeCount();
    if (mesh_count == 0)
    {
        m_meshNodeMapping.clear();
        return;
    }

    m_meshNodeMapping.resize(mesh_count);
    for (unsigned int m = 0; m < mesh_count; m++)
    {
        m_meshNodeMapping[m].m_nodeIndexInModel = m;
        auto mesh_node = mesh_node_tree.getMeshNode(m);
        if ((m_animationAsset) && (mesh_node))
        {
            m_meshNodeMapping[m].m_nodeIndexInAnimation =
                m_animationAsset->findMeshNodeIndex(mesh_node.value().get().getName());
        }
    }
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
    const std::shared_ptr<ModelPrimitive> model = cacheControlledModel();
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
    const std::shared_ptr<ModelPrimitive> model = cacheControlledModel();
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

std::shared_ptr<ModelPrimitive> ModelPrimitiveAnimator::cacheControlledModel()
{
    if ((!m_controlledModel.expired()) && (m_controlledModel.lock()->id() == m_controlledPrimitiveId)) return m_controlledModel.lock();
    m_controlledModel.reset();
    if (!m_controlledPrimitiveId) return nullptr;
    m_controlledModel = std::dynamic_pointer_cast<ModelPrimitive>(Primitives::Primitive::queryPrimitive(m_controlledPrimitiveId.value()));
    return m_controlledModel.lock();
}

bool ModelPrimitiveAnimator::updateMeshNodeTransform()
{
    if (m_meshNodeMapping.empty()) return false;
    const std::shared_ptr<ModelPrimitive> model = cacheControlledModel(); // m_controlledModel.lock();
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
