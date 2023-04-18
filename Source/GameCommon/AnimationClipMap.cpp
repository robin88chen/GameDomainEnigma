#include "AnimationClipMap.h"
#include <optional>

using namespace Enigma::GameCommon;
using namespace Enigma::Animators;

std::optional<AnimationClipMap::AnimClip> AnimationClipMap::FindAnimationClip(const std::string& name)
{
    auto iter = m_animClips.find(name);
    if (iter == m_animClips.end()) return std::nullopt;
    return iter->second;
}

void AnimationClipMap::InsertClip(const AnimClip& anim_clip)
{
    m_animClips[anim_clip.m_actionName] = anim_clip;
}

void AnimationClipMap::RemoveClip(const std::string& name)
{
    m_animClips.erase(name);
}
