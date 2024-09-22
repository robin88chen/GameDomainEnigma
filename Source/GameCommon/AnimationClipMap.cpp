#include "AnimationClipMap.h"
#include "AnimationClipMapAssembler.h"
#include "AnimationClipMapEvents.h"
#include "Frameworks/EventPublisher.h"
#include <optional>

using namespace Enigma::GameCommon;
using namespace Enigma::Renderables;
using namespace Enigma::Engine;

DEFINE_RTTI_OF_BASE(GameCommon, AnimationClipMap);

void AnimationClipMap::assemble(const std::shared_ptr<AnimationClipMapAssembler>& assembler) const
{
    assembler->clipMap(*this);
}

void AnimationClipMap::disassemble(const std::shared_ptr<AnimationClipMapDisassembler>& disassembler)
{
    m_animClips = disassembler->clipMap().animationClipMap();
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
