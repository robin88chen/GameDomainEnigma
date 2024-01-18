/*********************************************************************
 * \file   AnimatorDtos.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   January 2023
 *********************************************************************/
#ifndef ANI_ANIMATOR_DTOS_H
#define ANI_ANIMATOR_DTOS_H

#include "GameEngine/DtoDeserializer.h"
#include "GameEngine/GenericDto.h"
#include "Renderables/ModelPrimitive.h"
#include "GameEngine/AnimationAssetId.h"

namespace Enigma::Animators
{
    class ModelAnimatorPolicy;

    class ModelAnimatorDto
    {
    public:
        ModelAnimatorDto();
        ModelAnimatorDto(const Engine::GenericDto& dto);

        [[nodiscard]] const Engine::AnimatorId& id() const { return m_id; }
        Engine::AnimatorId& id() { return m_id; }
        [[nodiscard]] const Engine::FactoryDesc& factoryDesc() const { return m_factoryDesc; }
        Engine::FactoryDesc& factoryDesc() { return m_factoryDesc; }

        [[nodiscard]] const std::optional<Engine::AnimationAssetId>& animationAssetId() const { return m_animationAssetId; }
        std::optional<Engine::AnimationAssetId>& animationAssetId() { return m_animationAssetId; }
        [[nodiscard]] const Engine::GenericDtoCollection& skinOperators() const { return m_skinOperators; }
        Engine::GenericDtoCollection& skinOperators() { return m_skinOperators; }

        Engine::GenericDto toGenericDto();

        //std::shared_ptr<ModelAnimatorPolicy> convertToPolicy(const std::shared_ptr<Renderer::ModelPrimitive>& controlled,
          //  const std::shared_ptr<Engine::IDtoDeserializer>& deserializer);

    private:
        Engine::AnimatorId m_id;
        Engine::FactoryDesc m_factoryDesc;
        std::optional<Engine::AnimationAssetId> m_animationAssetId;
        Engine::GenericDtoCollection m_skinOperators;
    };

    class SkinOperatorDto
    {
    public:
        SkinOperatorDto();

        [[nodiscard]] const Engine::FactoryDesc& factoryDesc() const { return m_factoryDesc; }
        Engine::FactoryDesc& factoryDesc() { return m_factoryDesc; }

        [[nodiscard]] const std::optional<std::string>& SkinMeshName() const { return m_skinMeshName; }
        std::optional<std::string>& SkinMeshName() { return m_skinMeshName; }
        [[nodiscard]] const std::optional<std::string>& SkinMeshNodeName() const { return m_skinMeshNodeName; }
        std::optional<std::string>& SkinMeshNodeName() { return m_skinMeshNodeName; }
        [[nodiscard]] const std::vector<std::string>& BoneNodeNames() const { return m_boneNodeNames; }
        std::vector<std::string>& BoneNodeNames() { return m_boneNodeNames; }
        [[nodiscard]] const std::optional<std::vector<MathLib::Matrix4>>& NodeOffsets() const { return m_nodeOffsets; }
        std::optional<std::vector<MathLib::Matrix4>>& NodeOffsets() { return m_nodeOffsets; }

        static SkinOperatorDto fromGenericDto(const Engine::GenericDto& dto);
        Engine::GenericDto toGenericDto();

    private:
        Engine::FactoryDesc m_factoryDesc;
        std::optional<std::string> m_skinMeshName;
        std::optional<std::string> m_skinMeshNodeName;
        std::vector<std::string> m_boneNodeNames;
        std::optional<std::vector<MathLib::Matrix4>> m_nodeOffsets;
    };
}

#endif // ANIMATOR_DTOS_H
