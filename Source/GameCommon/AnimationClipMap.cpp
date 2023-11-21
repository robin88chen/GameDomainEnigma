#include "AnimationClipMap.h"
#include "AnimationClipMapDto.h"
#include "AnimationClipMapEvents.h"
#include "Frameworks/EventPublisher.h"
#include <optional>

using namespace Enigma::GameCommon;
using namespace Enigma::Animators;
using namespace Enigma::Engine;

DEFINE_RTTI_OF_BASE(GameCommon, AnimationClipMap);

AnimationClipMap::AnimationClipMap(const Engine::GenericDto& o)
{
    AnimationClipMapDto dto = AnimationClipMapDto::fromGenericDto(o);
    for (unsigned i = 0; i < dto.AnimNames().size(); ++i)
    {
        AnimClip clip(dto.AnimNames()[i], AnimationClip(dto.StartOffsets()[i], dto.LoopTimes()[i],
            static_cast<AnimationClip::WarpMode>(dto.WarpModes()[i]), dto.DivideIndices()[i]));
        m_animClips[clip.getName()] = clip;
    }
}

GenericDto AnimationClipMap::serializeDto() const
{
    AnimationClipMapDto dto;
    for (auto& [name, clip] : m_animClips)
    {
        dto.AnimNames().push_back(name);
        dto.StartOffsets().push_back(clip.GetClip().GetStartOffset());
        dto.LoopTimes().push_back(clip.GetClip().GetLoopTime());
        dto.WarpModes().push_back(static_cast<unsigned>(clip.GetClip().GetWarpMode()));
        dto.DivideIndices().push_back(clip.GetClip().GetDivideIndex());
    }
    return dto.toGenericDto();
}

stdext::optional_ref<AnimationClipMap::AnimClip> AnimationClipMap::FindAnimationClip(const std::string& name)
{
    auto iter = m_animClips.find(name);
    if (iter == m_animClips.end()) return std::nullopt;
    return iter->second;
}

std::optional<AnimationClipMap::AnimClip> AnimationClipMap::FindAnimationClip(const std::string& name) const
{
    auto iter = m_animClips.find(name);
    if (iter == m_animClips.end()) return std::nullopt;
    return iter->second;
}

void AnimationClipMap::InsertClip(const AnimClip& anim_clip)
{
    m_animClips[anim_clip.getName()] = anim_clip;
    Frameworks::EventPublisher::post(std::make_shared<AnimationClipMapChanged>(GetAnimationClipMap()));
}

void AnimationClipMap::RemoveClip(const std::string& name)
{
    m_animClips.erase(name);
    Frameworks::EventPublisher::post(std::make_shared<AnimationClipMapChanged>(GetAnimationClipMap()));
}
