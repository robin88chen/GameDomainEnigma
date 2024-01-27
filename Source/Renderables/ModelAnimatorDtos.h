/*********************************************************************
 * \file   ModelAnimatorDtos.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   January 2023
 *********************************************************************/
#ifndef MODEL_ANIMATOR_DTOS_H
#define MODEL_ANIMATOR_DTOS_H

#include "GameEngine/GenericDto.h"
#include "Animators/AnimationAssetId.h"
#include "Animators/AnimatorId.h"
#include "Primitives/PrimitiveId.h"
#include "MathLib/Matrix4.h"

namespace Enigma::Renderables
{
    class ModelAnimatorDto
    {
    public:
        ModelAnimatorDto();
        ModelAnimatorDto(const Engine::GenericDto& dto);

        [[nodiscard]] const Animators::AnimatorId& id() const { return m_id; }
        Animators::AnimatorId& id() { return m_id; }
        [[nodiscard]] const Engine::FactoryDesc& factoryDesc() const { return m_factoryDesc; }
        Engine::FactoryDesc& factoryDesc() { return m_factoryDesc; }

        [[nodiscard]] const std::optional<Primitives::PrimitiveId>& controlledPrimitiveId() const { return m_controlledPrimitiveId; }
        std::optional<Primitives::PrimitiveId>& controlledPrimitiveId() { return m_controlledPrimitiveId; }

        [[nodiscard]] const std::optional<Animators::AnimationAssetId>& animationAssetId() const { return m_animationAssetId; }
        std::optional<Animators::AnimationAssetId>& animationAssetId() { return m_animationAssetId; }
        [[nodiscard]] const Engine::GenericDtoCollection& skinOperators() const { return m_skinOperators; }
        Engine::GenericDtoCollection& skinOperators() { return m_skinOperators; }

        Engine::GenericDto toGenericDto();

    private:
        Animators::AnimatorId m_id;
        Engine::FactoryDesc m_factoryDesc;
        std::optional<Primitives::PrimitiveId> m_controlledPrimitiveId;
        std::optional<Animators::AnimationAssetId> m_animationAssetId;
        Engine::GenericDtoCollection m_skinOperators;
    };

    class SkinOperatorDto
    {
    public:
        SkinOperatorDto();
        SkinOperatorDto(const Engine::GenericDto& dto);

        [[nodiscard]] const Engine::FactoryDesc& factoryDesc() const { return m_factoryDesc; }
        Engine::FactoryDesc& factoryDesc() { return m_factoryDesc; }

        [[nodiscard]] const std::optional<Primitives::PrimitiveId>& skinMeshId() const { return m_skinMeshId; }
        std::optional<Primitives::PrimitiveId>& skinMeshId() { return m_skinMeshId; }
        [[nodiscard]] const std::optional<std::string>& skinMeshNodeName() const { return m_skinMeshNodeName; }
        std::optional<std::string>& skinMeshNodeName() { return m_skinMeshNodeName; }
        [[nodiscard]] const std::vector<std::string>& boneNodeNames() const { return m_boneNodeNames; }
        std::vector<std::string>& boneNodeNames() { return m_boneNodeNames; }
        [[nodiscard]] const std::optional<std::vector<MathLib::Matrix4>>& nodeOffsets() const { return m_nodeOffsets; }
        std::optional<std::vector<MathLib::Matrix4>>& nodeOffsets() { return m_nodeOffsets; }

        Engine::GenericDto toGenericDto();

    private:
        Engine::FactoryDesc m_factoryDesc;
        std::optional<Primitives::PrimitiveId> m_skinMeshId;
        std::optional<std::string> m_skinMeshNodeName;
        std::vector<std::string> m_boneNodeNames;
        std::optional<std::vector<MathLib::Matrix4>> m_nodeOffsets;
    };
}

#endif // ANIMATOR_DTOS_H
