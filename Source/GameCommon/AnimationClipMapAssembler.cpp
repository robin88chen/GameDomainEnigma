#include "AnimationClipMapAssembler.h"
#include "Renderables/AnimationClip.h"
#include <vector>
#include <string>

using namespace Enigma::GameCommon;

static std::string TOKEN_ANIM_NAMES = "AnimationNames";
static std::string TOKEN_START_OFFSETS = "StartOffsets";
static std::string TOKEN_LOOP_TIMES = "LoopTimes";
static std::string TOKEN_WARP_MODES = "WarpModes";
static std::string TOKEN_DIVIDE_INDICES = "DivideIndices";

AnimationClipMapAssembler::AnimationClipMapAssembler() : m_factoryDesc(Engine::FactoryDesc(AnimationClipMap::TYPE_RTTI))
{
}

AnimationClipMapAssembler::AnimationClipMapAssembler(const AnimationClipMap& clip_map) : m_factoryDesc(Engine::FactoryDesc(AnimationClipMap::TYPE_RTTI))
{
    clipMap(clip_map);
}

void AnimationClipMapAssembler::clipMap(const AnimationClipMap& clip_map)
{
    m_clips.clear();
    for (const auto& [name, clip] : clip_map.animationClipMap())
    {
        addClip(clip);
    }
}

void AnimationClipMapAssembler::addClip(const AnimationClipMap::AnimClip& clip)
{
    m_clips.emplace_back(clip);
}

Enigma::Engine::GenericDto AnimationClipMapAssembler::assemble() const
{
    std::vector<std::string> anim_names;
    std::vector<float> start_offsets;
    std::vector<float> loop_times;
    std::vector<unsigned> warp_modes;
    std::vector<unsigned> divide_indices;
    for (const auto& clip : m_clips)
    {
        anim_names.emplace_back(clip.getName());
        start_offsets.emplace_back(clip.clip().startOffset());
        loop_times.emplace_back(clip.clip().loopTime());
        warp_modes.emplace_back(static_cast<unsigned>(clip.clip().warpMode()));
        divide_indices.emplace_back(clip.clip().divideIndex());
    }

    Engine::GenericDto dto;
    dto.addRtti(m_factoryDesc);
    dto.addOrUpdate(TOKEN_ANIM_NAMES, anim_names);
    dto.addOrUpdate(TOKEN_START_OFFSETS, start_offsets);
    dto.addOrUpdate(TOKEN_LOOP_TIMES, loop_times);
    dto.addOrUpdate(TOKEN_WARP_MODES, warp_modes);
    dto.addOrUpdate(TOKEN_DIVIDE_INDICES, divide_indices);
    return dto;
}

AnimationClipMapDisassembler::AnimationClipMapDisassembler() : m_factoryDesc(Engine::FactoryDesc(AnimationClipMap::TYPE_RTTI))
{
}

AnimationClipMapDisassembler::AnimationClipMapDisassembler(const Engine::GenericDto& dto) : m_factoryDesc(Engine::FactoryDesc(AnimationClipMap::TYPE_RTTI))
{
    disassemble(dto);
}

void AnimationClipMapDisassembler::disassemble(const Engine::GenericDto& dto)
{
    std::vector<std::string> anim_names;
    std::vector<float> start_offsets;
    std::vector<float> loop_times;
    std::vector<unsigned> warp_modes;
    std::vector<unsigned> divide_indices;
    m_factoryDesc = dto.getRtti();
    if (auto v = dto.tryGetValue<std::vector<std::string>>(TOKEN_ANIM_NAMES)) anim_names = v.value();
    if (auto v = dto.tryGetValue<std::vector<float>>(TOKEN_START_OFFSETS)) start_offsets = v.value();
    if (auto v = dto.tryGetValue<std::vector<float>>(TOKEN_LOOP_TIMES)) loop_times = v.value();
    if (auto v = dto.tryGetValue<std::vector<unsigned>>(TOKEN_WARP_MODES)) warp_modes = v.value();
    if (auto v = dto.tryGetValue<std::vector<unsigned>>(TOKEN_DIVIDE_INDICES)) divide_indices = v.value();
    assert(anim_names.size() == start_offsets.size());
    assert(anim_names.size() == loop_times.size());
    assert(anim_names.size() == warp_modes.size());
    assert(anim_names.size() == divide_indices.size());
    for (size_t i = 0; i < anim_names.size(); ++i)
    {
        Renderables::AnimationClip clip(start_offsets[i], loop_times[i], static_cast<Renderables::AnimationClip::WarpMode>(warp_modes[i]), divide_indices[i]);
        m_clipMap.insertClip({ anim_names[i], clip });
    }
}

/*AnimationClipMapAssembler& AnimationClipMapAssembler::addClip(const std::string& name, const Renderables::AnimationClip& clip)
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
}*/
