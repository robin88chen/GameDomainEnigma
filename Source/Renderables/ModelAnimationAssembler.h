/*********************************************************************
 * \file   ModelAnimationAssembler.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   February 2024
 *********************************************************************/
#ifndef MODEL_ANIMATION_ASSEMBLER_H
#define MODEL_ANIMATION_ASSEMBLER_H

#include "Animators/AnimationAssetId.h"
#include "ModelAnimationDtos.h"
#include "AnimationTimeSRT.h"
#include "Animators/AnimatorPersistenceLevel.h"

namespace Enigma::Renderables
{
    class ModelAnimationAsset;

    class AnimationTimeSRTAssembler
    {
    public:
        AnimationTimeSRTAssembler& scaleKey(const AnimationTimeSRT::ScaleKey& key);
        AnimationTimeSRTAssembler& rotationKey(const AnimationTimeSRT::RotationKey& key);
        AnimationTimeSRTAssembler& translationKey(const AnimationTimeSRT::TranslateKey& key);

        Engine::GenericDto toGenericDto();

    protected:
        std::vector<AnimationTimeSRT::ScaleKey> m_scaleKeys;
        std::vector<AnimationTimeSRT::RotationKey> m_rotationKeys;
        std::vector<AnimationTimeSRT::TranslateKey> m_translationKeys;
        AnimationTimeSRTDto m_dto;
    };

    class ModelAnimationAssembler
    {
    public:
        ModelAnimationAssembler(const Animators::AnimationAssetId& id);

        ModelAnimationAssembler& nodeSRT(const std::string& node_name, const AnimationTimeSRTAssembler& assembler);
        ModelAnimationAssembler& asAsset(const std::string& name, const std::string& filename, const std::string& path_id);

        std::shared_ptr<ModelAnimationAsset> constitute(Animators::PersistenceLevel persistence_level);

    protected:
        Engine::GenericDto toGenericDto();

    protected:
        Animators::AnimationAssetId m_id;
        std::vector<std::tuple<std::string, AnimationTimeSRTAssembler>> m_nodeSRTs;
        ModelAnimationAssetDto m_dto;
    };
}

#endif // MODEL_ANIMATION_ASSEMBLER_H
