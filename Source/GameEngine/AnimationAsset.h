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
#include "AnimationAssetId.h"
#include "GenericDto.h"
#include <string>

namespace Enigma::Engine
{
    class AnimationAsset
    {
        DECLARE_EN_RTTI_OF_BASE
    public:
        AnimationAsset(const AnimationAssetId& id);
        AnimationAsset(const AnimationAsset&) = delete;
        AnimationAsset(AnimationAsset&&) = delete;
        virtual ~AnimationAsset();
        AnimationAsset& operator=(const AnimationAsset&) = delete;
        AnimationAsset& operator=(AnimationAsset&&) = delete;

        const AnimationAssetId& id() { return m_id; }

        virtual GenericDto serializeDto() = 0;

        static std::shared_ptr<AnimationAsset> queryAnimationAsset(const AnimationAssetId& id);

        const FactoryDesc& factoryDesc() const { return m_factoryDesc; }
        FactoryDesc& factoryDesc() { return m_factoryDesc; }

        virtual float getAnimationLengthInSecond() = 0;

    protected:
        AnimationAssetId m_id;
        FactoryDesc m_factoryDesc;
    };
}

#endif // _ANIMATION_ASSET_H
