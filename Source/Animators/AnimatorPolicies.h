/*********************************************************************
 * \file   AnimatorPolicies.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   January 2023
 *********************************************************************/
#ifndef _ANIMATOR_POLICIES_H
#define _ANIMATOR_POLICIES_H

#include "Renderer/ModelPrimitive.h"
#include "AnimatorDtos.h"
#include <memory>

namespace Enigma::Animators
{
    class AnimationAssetPolicy;

    class ModelAnimatorPolicy
    {
    public:
        ModelAnimatorPolicy() : m_ruid(Frameworks::Ruid::Generate()), m_assetFactory(Engine::Animator::TYPE_RTTI.GetName()) {}
        ModelAnimatorPolicy(const std::shared_ptr<Renderer::ModelPrimitive>& controlled, const std::shared_ptr<AnimationAssetPolicy>& asset_policy)
            : m_ruid(Frameworks::Ruid::Generate()), m_assetFactory(Engine::Animator::TYPE_RTTI.GetName()),
                m_controlledPrimitive(controlled), m_assetPolicy(asset_policy)  {}

        [[nodiscard]] const Frameworks::Ruid& GetRuid() const { return m_ruid; }
        [[nodiscard]] const Engine::FactoryDesc& AssetFactoryDesc() const { return m_assetFactory; }
        Engine::FactoryDesc& AssetFactoryDesc() { return m_assetFactory; }
        [[nodiscard]] const std::shared_ptr<Renderer::ModelPrimitive>& ControlledPrimitive() const { return m_controlledPrimitive; }
        [[nodiscard]] const std::shared_ptr<AnimationAssetPolicy>& GetAssetPolicy() const { return m_assetPolicy; }
        [[nodiscard]] const std::vector<SkinOperatorDto>& SkinOperators() const { return m_skinOperators; }
        std::vector<SkinOperatorDto>& SkinOperators() { return m_skinOperators; }

    private:
        Frameworks::Ruid m_ruid;
        Engine::FactoryDesc m_assetFactory;
        std::shared_ptr<Renderer::ModelPrimitive> m_controlledPrimitive;
        std::shared_ptr<AnimationAssetPolicy> m_assetPolicy;
        std::vector<SkinOperatorDto> m_skinOperators;
    };
}

#endif // _ANIMATOR_POLICIES_H
