#include "ModelPrimitiveAnimator.h"
#include "Platforms/PlatformLayer.h"
#include "Renderer/ModelPrimitive.h"
#include "ModelAnimationAsset.h"

using namespace Enigma::Animators;
using namespace Enigma::Frameworks;
using namespace Enigma::Renderer;
using namespace Enigma::Engine;

DEFINE_RTTI(Animators, ModelPrimitiveAnimator, Animator);

ModelPrimitiveAnimator::ModelPrimitiveAnimator() : Animator()
{
    m_animationAsset = nullptr;
    m_meshNodeMapping.clear();
    //todo: skin mesh
    //m_skinAnimOperators.clear();

    m_remainFadingTime = 0.0f;
    m_fadingTime = 0.1f;
    m_isFading = false;
    m_isOnPlay = false;
}

ModelPrimitiveAnimator::ModelPrimitiveAnimator(const ModelPrimitiveAnimator& ani) : Animator()
{
    m_animationAsset = ani.m_animationAsset;
    m_meshNodeMapping = ani.m_meshNodeMapping;
    //todo : skin mesh
    m_remainFadingTime = 0.0f;
    m_fadingTime = 0.1f;
    m_isFading = false;
    m_isOnPlay = false;
}

ModelPrimitiveAnimator::~ModelPrimitiveAnimator()
{
    m_animationAsset = nullptr;
    m_meshNodeMapping.clear();
    //todo : skin mesh
}

ModelPrimitiveAnimator& ModelPrimitiveAnimator::operator=(const ModelPrimitiveAnimator& ani)
{
    m_animationAsset = ani.m_animationAsset;
    m_meshNodeMapping = ani.m_meshNodeMapping;
    //todo : skin mesh
    m_remainFadingTime = 0.0f;
    m_fadingTime = 0.1f;
    m_isFading = false;
    m_isOnPlay = false;

    return *this;
}

Animator::HasUpdated ModelPrimitiveAnimator::Update(Frameworks::Timer* timer)
{
    if (FATAL_LOG_EXPR(!timer)) return HasUpdated::False;
    if (!m_isOnPlay) return HasUpdated::False;

    const float elapse_time = timer->GetElapseTime();
    auto next_to_stop = m_currentAnimClip.Update(elapse_time);

    if (m_isFading)
    {
        m_fadeInAnimClip.Update(elapse_time);
        m_remainFadingTime -= elapse_time;
    }

    const auto res = TimeValueUpdate();

    if (static_cast<bool>(next_to_stop))
    {
        m_isOnPlay = false;
    }
    return res ? HasUpdated::True : HasUpdated::False;
}

void ModelPrimitiveAnimator::Reset()
{
    m_currentAnimClip.Reset();
}
void ModelPrimitiveAnimator::LinkAnimationAsset(const std::shared_ptr<ModelAnimationAsset>& anim_asset)
{
    m_animationAsset = anim_asset;
    CalculateMeshNodeMapping();
}

void ModelPrimitiveAnimator::CalculateMeshNodeMapping()
{
    unsigned mesh_count = 0;
    if (m_controlledPrimitive.expired())
    {
        m_meshNodeMapping.clear();
        return;
    }
    ModelPrimitivePtr model = m_controlledPrimitive.lock();
    assert(model);
    assert(model->GetMeshNodeTree().GetMeshNodeCount() > 0);
    if (mesh_count < model->GetMeshNodeTree().GetMeshNodeCount())
    {
        mesh_count = model->GetMeshNodeTree().GetMeshNodeCount();
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
        auto mesh_node = model->GetMeshNodeTree().GetMeshNode(m);
        if ((m_animationAsset) && (mesh_node))
        {
            m_meshNodeMapping[m].m_nodeIndexInAnimation =
                m_animationAsset->FindMeshNodeIndex(mesh_node.value().get().GetName());
        }
    }
}

void ModelPrimitiveAnimator::PlayAnimation(const AnimationClip& clip)
{
    m_currentAnimClip = clip;
    m_isOnPlay = true;
}

void ModelPrimitiveAnimator::FadeInAnimation(float fading_time, const AnimationClip& clip)
{
    if (!m_isOnPlay)
    {
        PlayAnimation(clip);
    }
    else
    {
        if (m_currentAnimClip.RemainLoopTime() < fading_time)  // 剩下的時間不夠做 fading
        {
            fading_time = m_currentAnimClip.RemainLoopTime();
        }
        m_fadeInAnimClip = clip;
        m_fadingTime = fading_time;
        m_remainFadingTime = fading_time;
        m_isFading = true;
    }
}

void ModelPrimitiveAnimator::StopAnimation()
{
    m_isOnPlay = false;
}

bool ModelPrimitiveAnimator::TimeValueUpdate()
{
    if (m_isFading)
    {
        const bool hasUpdate = UpdateMeshNodeTransformWithFading();
        if (!hasUpdate) return false;
    }
    else
    {  // none fading animation
        const bool hasUpdate = UpdateMeshNodeTransform();
        if (!hasUpdate) return false;
    }
    if (m_controlledPrimitive.expired()) return false;
    ModelPrimitivePtr model = m_controlledPrimitive.lock();
    if (!model) return false;
    //todo : skin mesh
    /*if (m_skinAnimOperators.size() == 1)
    {
        m_skinAnimOperators[0]->UpdateSkinMeshBoneMatrix(model->GetMeshNodeTree());
    }
    else
    {
        SkinAnimationOperatorVector::iterator iter = m_skinAnimOperators.begin();
        while (iter != m_skinAnimOperators.end())
        {
            if (*iter) (*iter)->UpdateSkinMeshBoneMatrix(model->GetMeshNodeTree());
            ++iter;
        }
    }*/

    return true;
}

bool ModelPrimitiveAnimator::UpdateMeshNodeTransformWithFading()
{
    if (m_meshNodeMapping.empty()) return false;
    if (m_controlledPrimitive.expired()) return false;
    const ModelPrimitivePtr model = m_controlledPrimitive.lock();
    if (!model) return false;
    if (model->GetMeshNodeTree().GetMeshNodeCount() == 0) return false;
    if (!m_animationAsset) return false;

    const float current_time_value = m_currentAnimClip.CurrentTimeValue();
    float fading_weight = m_remainFadingTime / m_fadingTime;
    if (m_remainFadingTime <= 0.0f) fading_weight = 0.0f;

    float fadein_time_value = m_fadeInAnimClip.CurrentTimeValue();
    const unsigned mesh_count = model->GetMeshNodeTree().GetMeshNodeCount();
    for (unsigned m = 0; m < mesh_count; m++)
    {
        auto mesh_index = m_meshNodeMapping[m].m_nodeIndexInModel;
        if (!mesh_index) continue;
        auto mesh_node = model->GetMeshNodeTree().GetMeshNode(mesh_index.value());
        if (!mesh_node) continue;
        if (auto ani_index = m_meshNodeMapping[m].m_nodeIndexInAnimation)
        {
            model->UpdateMeshNodeLocalTransform(mesh_index.value(),
                m_animationAsset->CalculateFadedTransformMatrix
                (ani_index.value(), current_time_value, fadein_time_value, fading_weight));
        }
        else
        {
            // 沒有這個node 的 animation, 用 mesh node 的原始local transform 更新
            model->UpdateMeshNodeLocalTransform(mesh_index.value(), mesh_node.value().get().GetLocalTransform());
        }
    }

    if (m_remainFadingTime <= 0.0f)
    { // clear fading state
        m_isFading = false;
        m_currentAnimClip = m_fadeInAnimClip;
    }
    return true;
}

bool ModelPrimitiveAnimator::UpdateMeshNodeTransform()
{
    if (m_meshNodeMapping.empty()) return false;
    if (m_controlledPrimitive.expired()) return false;
    const ModelPrimitivePtr model = m_controlledPrimitive.lock();
    if (!model) return false;
    if (model->GetMeshNodeTree().GetMeshNodeCount() == 0) return false;
    if (!m_animationAsset) return false;

    const float current_time_value = m_currentAnimClip.CurrentTimeValue();
    const unsigned mesh_count = model->GetMeshNodeTree().GetMeshNodeCount();
    for (unsigned m = 0; m < mesh_count; m++)
    {
        auto mesh_index = m_meshNodeMapping[m].m_nodeIndexInModel;
        if (!mesh_index) continue;
        auto mesh_node = model->GetMeshNodeTree().GetMeshNode(mesh_index.value());
        if (!mesh_node) continue;
        if (auto ani_index = m_meshNodeMapping[m].m_nodeIndexInAnimation)
        {
            model->UpdateMeshNodeLocalTransform(mesh_index.value(),
                m_animationAsset->CalculateTransformMatrix(ani_index.value(), current_time_value));
        }
        else
        {
            // 沒有這個node 的 animation, 用 mesh node 的原始local transform 更新
            model->UpdateMeshNodeLocalTransform(mesh_index.value(), mesh_node.value().get().GetLocalTransform());
        }
    }
    return true;
}
