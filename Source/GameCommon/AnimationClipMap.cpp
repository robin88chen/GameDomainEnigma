#include "AnimationClipMap.h"
#include "AnimationClipMapDto.h"
#include "AnimationClipMapEvents.h"
#include "Frameworks/EventPublisher.h"
#include <optional>

using namespace Enigma::GameCommon;
using namespace Enigma::Renderables;
using namespace Enigma::Engine;

DEFINE_RTTI_OF_BASE(GameCommon, AnimationClipMap);

AnimationClipMap::AnimationClipMap(const Engine::GenericDto& o)
{
    AnimationClipMapDto dto(o);
    for (unsigned i = 0; i < dto.animationNames().size(); ++i)
    {
        AnimClip clip(dto.animationNames()[i], AnimationClip(dto.startOffsets()[i], dto.loopTimes()[i],
            static_cast<AnimationClip::WarpMode>(dto.warpModes()[i]), dto.divideIndices()[i]));
        m_animClips[clip.getName()] = clip;
    }
}

GenericDto AnimationClipMap::serializeDto() const
{
    AnimationClipMapDto dto;
    for (auto& [name, clip] : m_animClips)
    {
        dto.animationNames().push_back(name);
        dto.startOffsets().push_back(clip.clip().startOffset());
        dto.loopTimes().push_back(clip.clip().loopTime());
        dto.warpModes().push_back(static_cast<unsigned>(clip.clip().warpMode()));
        dto.divideIndices().push_back(clip.clip().divideIndex());
    }
    return dto.toGenericDto();
}

stdext::optional_ref<AnimationClipMap::AnimClip> AnimationClipMap::findAnimationClip(const std::string& name)
{
    auto iter = m_animClips.find(name);
    if (iter == m_animClips.end()) return std::nullopt;
    return iter->second;
}

std::optional<AnimationClipMap::AnimClip> AnimationClipMap::findAnimationClip(const std::string& name) const
{
    auto iter = m_animClips.find(name);
    if (iter == m_animClips.end()) return std::nullopt;
    return iter->second;
}

void AnimationClipMap::insertClip(const AnimClip& anim_clip)
{
    m_animClips[anim_clip.getName()] = anim_clip;
    Frameworks::EventPublisher::enqueue(std::make_shared<AnimationClipMapChanged>(animationClipMap()));
}

void AnimationClipMap::removeClip(const std::string& name)
{
    m_animClips.erase(name);
    Frameworks::EventPublisher::enqueue(std::make_shared<AnimationClipMapChanged>(animationClipMap()));
}
