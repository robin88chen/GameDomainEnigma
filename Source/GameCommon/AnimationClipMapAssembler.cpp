#include "AnimationClipMapAssembler.h"

using namespace Enigma::GameCommon;

AnimationClipMapAssembler& AnimationClipMapAssembler::addClip(const std::string& name, const Renderables::AnimationClip& clip)
{
    m_dto.animationNames().emplace_back(name);
    m_dto.startOffsets().emplace_back(clip.startOffset());
    m_dto.loopTimes().emplace_back(clip.loopTime());
    m_dto.warpModes().emplace_back(static_cast<unsigned>(clip.warpMode()));
    m_dto.divideIndices().emplace_back(clip.divideIndex());
    return *this;
}

Enigma::Engine::GenericDto AnimationClipMapAssembler::toGenericDto() const
{
    return m_dto.toGenericDto();
}
