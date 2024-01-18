/*********************************************************************
 * \file   AnimatorPolicies.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   January 2023
 *********************************************************************/
#ifndef _ANIMATOR_POLICIES_H
#define _ANIMATOR_POLICIES_H

#include "Renderables/ModelPrimitive.h"
#include "AnimatorDtos.h"
#include <memory>

namespace Enigma::Animators
{
    class AnimationAssetPolicy;

    class ModelAnimatorPolicy
    {
    public:
        ModelAnimatorPolicy() : m_ruid(Frameworks::Ruid::generate()), m_assetFactory(Engine::Animator::TYPE_RTTI.getName()) {}
        ModelAnimatorPolicy(const std::shared_ptr<Renderables::ModelPrimitive>& controlled, const std::shared_ptr<AnimationAssetPolicy>& asset_policy)
            : m_ruid(Frameworks::Ruid::generate()), m_assetFactory(Engine::Animator::TYPE_RTTI.getName()),
            m_controlledPrimitive(controlled), m_assetPolicy(asset_policy) {}

        [[nodiscard]] const Frameworks::Ruid& getRuid() const { return m_ruid; }
        [[nodiscard]] const Engine::FactoryDesc& AssetFactoryDesc() const { return m_assetFactory; }
        Engine::FactoryDesc& AssetFactoryDesc() { return m_assetFactory; }
        [[nodiscard]] const std::shared_ptr<Renderables::ModelPrimitive>& ControlledPrimitive() const { return m_controlledPrimitive; }
        [[nodiscard]] const std::shared_ptr<AnimationAssetPolicy>& GetAssetPolicy() const { return m_assetPolicy; }
        [[nodiscard]] const std::vector<SkinOperatorDto>& SkinOperators() const { return m_skinOperators; }
        std::vector<SkinOperatorDto>& SkinOperators() { return m_skinOperators; }

    private:
        Frameworks::Ruid m_ruid;
        Engine::FactoryDesc m_assetFactory;
        std::shared_ptr<Renderables::ModelPrimitive> m_controlledPrimitive;
        std::shared_ptr<AnimationAssetPolicy> m_assetPolicy;
        std::vector<SkinOperatorDto> m_skinOperators;
    };
}

#endif // _ANIMATOR_POLICIES_H
