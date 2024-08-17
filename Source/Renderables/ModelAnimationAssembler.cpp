#include "ModelAnimationAssembler.h"
#include "Animators/AnimationAssetQueries.h"
#include "ModelAnimationAsset.h"
#include <algorithm>

using namespace Enigma::Renderables;

AnimationTimeSRTAssembler& AnimationTimeSRTAssembler::scaleKey(const AnimationTimeSRT::ScaleKey& key)
{
    m_scaleKeys.push_back(key);
    return *this;
}

AnimationTimeSRTAssembler& AnimationTimeSRTAssembler::rotationKey(const AnimationTimeSRT::RotationKey& key)
{
    m_rotationKeys.push_back(key);
    return *this;
}

AnimationTimeSRTAssembler& AnimationTimeSRTAssembler::translationKey(const AnimationTimeSRT::TranslateKey& key)
{
    m_translationKeys.push_back(key);
    return *this;
}

Enigma::Engine::GenericDto AnimationTimeSRTAssembler::toGenericDto()
{
    std::sort(m_scaleKeys.begin(), m_scaleKeys.end(), [](const AnimationTimeSRT::ScaleKey& a, const AnimationTimeSRT::ScaleKey& b) { return a.m_time < b.m_time; });
    std::sort(m_rotationKeys.begin(), m_rotationKeys.end(), [](const AnimationTimeSRT::RotationKey& a, const AnimationTimeSRT::RotationKey& b) { return a.m_time < b.m_time; });
    std::sort(m_translationKeys.begin(), m_translationKeys.end(), [](const AnimationTimeSRT::TranslateKey& a, const AnimationTimeSRT::TranslateKey& b) { return a.m_time < b.m_time; });
    m_dto.scaleTimeKeys().clear();
    for (auto& key : m_scaleKeys)
    {
        auto values = key.values();
        m_dto.scaleTimeKeys().insert(m_dto.scaleTimeKeys().end(), values.begin(), values.end());
    }
    m_dto.rotateTimeKeys().clear();
    for (auto& key : m_rotationKeys)
    {
        auto values = key.values();
        m_dto.rotateTimeKeys().insert(m_dto.rotateTimeKeys().end(), values.begin(), values.end());
    }
    m_dto.translateTimeKeys().clear();
    for (auto& key : m_translationKeys)
    {
        auto values = key.values();
        m_dto.translateTimeKeys().insert(m_dto.translateTimeKeys().end(), values.begin(), values.end());
    }

    return m_dto.toGenericDto();
}

ModelAnimationAssembler::ModelAnimationAssembler(const Animators::AnimationAssetId& id)
{
    m_id = id;
    //m_dto.id(id);
}

ModelAnimationAssembler& ModelAnimationAssembler::nodeSRT(const std::string& node_name, const AnimationTimeSRTAssembler& assembler)
{
    m_nodeSRTs.emplace_back(node_name, assembler);
    return *this;
}

ModelAnimationAssembler& ModelAnimationAssembler::asAsset(const std::string& name, const std::string& filename, const std::string& path_id)
{
    //Engine::FactoryDesc fd = m_dto.factoryDesc();
    //fd.ClaimAsResourceAsset(name, filename, path_id);
    //m_dto.factoryDesc(fd);
    return *this;
}

Enigma::Engine::GenericDto ModelAnimationAssembler::toGenericDto()
{
    m_dto.meshNodeNames().clear();
    m_dto.timeSRTs().clear();
    for (auto& nodeSRT : m_nodeSRTs)
    {
        m_dto.meshNodeNames().emplace_back(std::get<std::string>(nodeSRT));
        m_dto.timeSRTs().emplace_back(std::get<AnimationTimeSRTAssembler>(nodeSRT).toGenericDto());
    }
    return m_dto.toGenericDto();
}

std::shared_ptr<ModelAnimationAsset> ModelAnimationAssembler::constitute()
{
    return std::dynamic_pointer_cast<ModelAnimationAsset>(std::make_shared<Animators::RequestAnimationAssetConstitution>(m_id, toGenericDto())->dispatch());
}

