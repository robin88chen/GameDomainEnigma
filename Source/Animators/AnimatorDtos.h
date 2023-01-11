/*********************************************************************
 * \file   AnimatorDtos.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   January 2023
 *********************************************************************/
#ifndef ANIMATOR_DTOS_H
#define ANIMATOR_DTOS_H

#include "GameEngine/DtoDeserializer.h"
#include "GameEngine/GenericDto.h"
#include "Renderer/ModelPrimitive.h"

namespace Enigma::Animators
{
    class ModelAnimatorPolicy;

    class ModelAnimatorDto
    {
    public:
        ModelAnimatorDto();

        [[nodiscard]] const std::string& AssetName() const { return m_assetName; }
        std::string& AssetName() { return m_assetName; }
        [[nodiscard]] const std::optional<Engine::GenericDto>& AnimationAssetDto() const { return m_animationAssetDto; }
        std::optional<Engine::GenericDto>& AnimationAssetDto() { return m_animationAssetDto; }
        [[nodiscard]] const Engine::FactoryDesc& AssetFactoryDesc() const { return m_assetFactory; }
        Engine::FactoryDesc& AssetFactoryDesc() { return m_assetFactory; }

        static ModelAnimatorDto FromGenericDto(const Engine::GenericDto& dto);
        Engine::GenericDto ToGenericDto();

        std::shared_ptr<ModelAnimatorPolicy> ConvertToPolicy(const std::shared_ptr<Renderer::ModelPrimitive>& controlled,
            const std::shared_ptr<Engine::IDtoDeserializer>& deserializer);

    private:
        std::string m_assetName;
        std::optional<Engine::GenericDto> m_animationAssetDto;
        Engine::FactoryDesc m_assetFactory;
    };
}

#endif // ANIMATOR_DTOS_H
