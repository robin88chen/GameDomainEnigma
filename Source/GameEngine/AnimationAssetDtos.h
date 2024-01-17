/*********************************************************************
 * \file   AnimationAssetDtos.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   January 2024
 *********************************************************************/
#ifndef ANIMATION_ASSET_DTOS_H
#define ANIMATION_ASSET_DTOS_H

#include "AnimationAssetId.h"
#include "FactoryDesc.h"

namespace Enigma::Engine
{
    class AnimationAssetDto
    {
    public:
        AnimationAssetDto();

        [[nodiscard]] const AnimationAssetId& id() const { return m_id; }
        AnimationAssetId& id() { return m_id; }

        [[nodiscard]] const FactoryDesc& factoryDesc() const { return m_factoryDesc; }
        FactoryDesc& factoryDesc() { return m_factoryDesc; }

    protected:
        AnimationAssetId m_id;
        FactoryDesc m_factoryDesc;
    };
}

#endif // ANIMATION_ASSET_DTOS_H
