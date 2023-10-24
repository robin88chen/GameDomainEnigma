/*********************************************************************
 * \file   AnimationAsset.h
 * \brief  animation asset base class, entity, 因為資料龐大, use shared_ptr
 *
 * \author Lancelot 'Robin' Chen
 * \date   January 2023
 *********************************************************************/
#ifndef _ANIMATION_ASSET_H
#define _ANIMATION_ASSET_H

#include "Frameworks/Rtti.h"
#include "GameEngine/FactoryDesc.h"
#include <string>
#include <memory>

namespace Enigma::Animators
{
    class AnimationAsset
    {
        DECLARE_EN_RTTI_OF_BASE
    public:
        AnimationAsset(const std::string& name);
        AnimationAsset(const AnimationAsset&) = delete;
        AnimationAsset(AnimationAsset&&) = delete;
        virtual ~AnimationAsset();
        AnimationAsset& operator=(const AnimationAsset&) = delete;
        AnimationAsset& operator=(AnimationAsset&&) = delete;

        const std::string& getName() { return m_name; }

        const Engine::FactoryDesc& TheFactoryDesc() const { return m_factoryDesc; }
        Engine::FactoryDesc& TheFactoryDesc() { return m_factoryDesc; }

        virtual float GetAnimationLengthInSecond() = 0;

    protected:
        std::string m_name;
        Engine::FactoryDesc m_factoryDesc;
    };
    using AnimationAssetPtr = std::shared_ptr<AnimationAsset>;
}

#endif // _ANIMATION_ASSET_H
