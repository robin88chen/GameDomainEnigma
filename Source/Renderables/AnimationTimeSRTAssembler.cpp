#include "AnimationTimeSRTAssembler.h"
#include "AnimationTimeSRT.h"
#include <algorithm>

using namespace Enigma::Renderables;

static std::string TOKEN_SCALE_TIME_KEYS = "ScaleTimeKeys";
static std::string TOKEN_ROTATE_TIME_KEYS = "RotateTimeKeys";
static std::string TOKEN_TRANSLATE_TIME_KEYS = "TranslateTimeKeys";

void AnimationTimeSRTAssembler::addScaleKey(const AnimationTimeSRT::ScaleKey& key)
{
    m_scaleKeys.push_back(key);
}

void AnimationTimeSRTAssembler::addRotationKey(const AnimationTimeSRT::RotationKey& key)
{
    m_rotationKeys.push_back(key);
}

void AnimationTimeSRTAssembler::addTranslationKey(const AnimationTimeSRT::TranslateKey& key)
{
    m_translationKeys.push_back(key);
}

Enigma::Engine::GenericDto AnimationTimeSRTAssembler::assembleWithSorted()
{
    std::sort(m_scaleKeys.begin(), m_scaleKeys.end(), [](const AnimationTimeSRT::ScaleKey& a, const AnimationTimeSRT::ScaleKey& b) { return a.m_time < b.m_time; });
    std::sort(m_rotationKeys.begin(), m_rotationKeys.end(), [](const AnimationTimeSRT::RotationKey& a, const AnimationTimeSRT::RotationKey& b) { return a.m_time < b.m_time; });
    std::sort(m_translationKeys.begin(), m_translationKeys.end(), [](const AnimationTimeSRT::TranslateKey& a, const AnimationTimeSRT::TranslateKey& b) { return a.m_time < b.m_time; });
    std::vector<float> scaleTimeKeys;
    for (auto& key : m_scaleKeys)
    {
        auto values = key.values();
        scaleTimeKeys.insert(scaleTimeKeys.end(), values.begin(), values.end());
    }
    std::vector<float> rotateTimeKeys;
    for (auto& key : m_rotationKeys)
    {
        auto values = key.values();
        rotateTimeKeys.insert(rotateTimeKeys.end(), values.begin(), values.end());
    }
    std::vector<float> translateTimeKeys;
    for (auto& key : m_translationKeys)
    {
        auto values = key.values();
        translateTimeKeys.insert(translateTimeKeys.end(), values.begin(), values.end());
    }
    Engine::GenericDto dto;
    dto.addOrUpdate(TOKEN_SCALE_TIME_KEYS, scaleTimeKeys);
    dto.addOrUpdate(TOKEN_ROTATE_TIME_KEYS, rotateTimeKeys);
    dto.addOrUpdate(TOKEN_TRANSLATE_TIME_KEYS, translateTimeKeys);
    return dto;
}

std::shared_ptr<AnimationTimeSRTAssembler> AnimationTimeSRTAssembler::make(const AnimationTimeSRT& srt)
{
    std::shared_ptr<AnimationTimeSRTAssembler> assembler = std::make_shared<AnimationTimeSRTAssembler>();
    srt.assemble(assembler);
    return assembler;
}

AnimationTimeSRTDisassembler::AnimationTimeSRTDisassembler(const Engine::GenericDto& dto)
{
    disassemble(dto);
}

void AnimationTimeSRTDisassembler::disassemble(const Engine::GenericDto& dto)
{
    if (const auto v = dto.tryGetValue<std::vector<float>>(TOKEN_SCALE_TIME_KEYS))
    {
        m_scaleKeys.clear();
        const std::vector<float>& values = v.value();
        assert(values.size() % 4 == 0);
        for (size_t i = 0; i < values.size(); i += 4)
        {
            m_scaleKeys.emplace_back(values[i], values[i + 1], values[i + 2], values[i + 3]);
        }
    }
    if (const auto v = dto.tryGetValue<std::vector<float>>(TOKEN_ROTATE_TIME_KEYS))
    {
        m_rotationKeys.clear();
        const std::vector<float>& values = v.value();
        assert(values.size() % 5 == 0);
        for (size_t i = 0; i < values.size(); i += 5)
        {
            m_rotationKeys.emplace_back(values[i], values[i + 1], values[i + 2], values[i + 3], values[i + 4]);
        }
    }
    if (const auto v = dto.tryGetValue<std::vector<float>>(TOKEN_TRANSLATE_TIME_KEYS))
    {
        m_translationKeys.clear();
        const std::vector<float>& values = v.value();
        assert(values.size() % 4 == 0);
        for (size_t i = 0; i < values.size(); i += 4)
        {
            m_translationKeys.emplace_back(values[i], values[i + 1], values[i + 2], values[i + 3]);
        }
    }
}

AnimationTimeSRT AnimationTimeSRTDisassembler::srt(const Engine::GenericDto& dto)
{
    AnimationTimeSRT srt;
    std::shared_ptr<AnimationTimeSRTDisassembler> disassembler = std::make_shared<AnimationTimeSRTDisassembler>(dto);
    srt.disassemble(disassembler);
    return srt;
}
