#include "AnimationClipMap.h"
#include "AnimationClipMapDto.h"
#include <optional>

using namespace Enigma::GameCommon;
using namespace Enigma::Animators;
using namespace Enigma::Engine;

DEFINE_RTTI_OF_BASE(GameCommon, AnimationClipMap);

AnimationClipMap::AnimationClipMap(const Engine::GenericDto& o)
{
    AnimationClipMapDto dto = AnimationClipMapDto::FromGenericDto(o);
    for (unsigned i = 0; i < dto.AnimNames().size(); ++i)
    {
        AnimClip clip;
        clip.m_actionName = dto.AnimNames()[i];
        clip.m_animClip = AnimationClip(dto.StartOffsets()[i], dto.LoopTimes()[i],
            static_cast<AnimationClip::WarpMode>(dto.WarpModes()[i]), dto.DivideIndices()[i]);
        m_animClips[clip.m_actionName] = clip;
    }
}

GenericDto AnimationClipMap::ToGenericDto() const
{
    AnimationClipMapDto dto;
    for (auto& [name, clip] : m_animClips)
    {
        dto.AnimNames().push_back(name);
        dto.StartOffsets().push_back(clip.m_animClip.GetStartOffset());
        dto.LoopTimes().push_back(clip.m_animClip.GetLoopTime());
        dto.WarpModes().push_back(static_cast<unsigned>(clip.m_animClip.GetWarpMode()));
        dto.DivideIndices().push_back(clip.m_animClip.GetDivideIndex());
    }
    return dto.ToGenericDto();
}

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
