#include "AnimationClipMapDto.h"
#include "AnimationClipMap.h"

using namespace Enigma::GameCommon;
using namespace Enigma::Animators;
using namespace Enigma::Engine;

static std::string TOKEN_ANIM_NAMES = "AnimationNames";
static std::string TOKEN_START_OFFSETS = "StartOffsets";
static std::string TOKEN_LOOP_TIMES = "LoopTimes";
static std::string TOKEN_WARP_MODES = "WarpModes";
static std::string TOKEN_DIVIDE_INDICES = "DivideIndices";

AnimationClipMapDto::AnimationClipMapDto() : m_factoryDesc(AnimationClipMap::TYPE_RTTI.getName())
{
}

AnimationClipMapDto AnimationClipMapDto::FromGenericDto(const Engine::GenericDto& dto)
{
    AnimationClipMapDto anim;
    anim.m_factoryDesc = dto.GetRtti();
    if (auto v = dto.TryGetValue<std::vector<std::string>>(TOKEN_ANIM_NAMES)) anim.AnimNames() = v.value();
    if (auto v = dto.TryGetValue<std::vector<float>>(TOKEN_START_OFFSETS)) anim.StartOffsets() = v.value();
    if (auto v = dto.TryGetValue<std::vector<float>>(TOKEN_LOOP_TIMES)) anim.LoopTimes() = v.value();
    if (auto v = dto.TryGetValue<std::vector<unsigned>>(TOKEN_WARP_MODES)) anim.WarpModes() = v.value();
    if (auto v = dto.TryGetValue<std::vector<unsigned>>(TOKEN_DIVIDE_INDICES)) anim.DivideIndices() = v.value();
    return anim;
}

GenericDto AnimationClipMapDto::ToGenericDto() const
{
    GenericDto dto;
    dto.AddRtti(m_factoryDesc);
    dto.AddOrUpdate(TOKEN_ANIM_NAMES, m_animNames);
    dto.AddOrUpdate(TOKEN_START_OFFSETS, m_startOffsets);
    dto.AddOrUpdate(TOKEN_LOOP_TIMES, m_loopTimes);
    dto.AddOrUpdate(TOKEN_WARP_MODES, m_warpModes);
    dto.AddOrUpdate(TOKEN_DIVIDE_INDICES, m_divideIndices);
    return dto;
}
