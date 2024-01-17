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
#include "FactoryDesc.h"
#include <string>
#include <memory>

namespace Enigma::Engine
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

        const FactoryDesc& factoryDesc() const { return m_factoryDesc; }
        FactoryDesc& factoryDesc() { return m_factoryDesc; }

        virtual float getAnimationLengthInSecond() = 0;

    protected:
        std::string m_name;
        FactoryDesc m_factoryDesc;
    };
}

#endif // _ANIMATION_ASSET_H
