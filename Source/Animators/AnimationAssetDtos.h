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
#include "GameEngine/FactoryDesc.h"

namespace Enigma::Animators
{
    class AnimationAssetDto
    {
    public:
        AnimationAssetDto();

        [[nodiscard]] const AnimationAssetId& id() const { return m_id; }
        void id(const AnimationAssetId& id) { m_id = id; }

        [[nodiscard]] const Engine::FactoryDesc& factoryDesc() const { return m_factoryDesc; }
        void factoryDesc(const Engine::FactoryDesc& factoryDesc) { m_factoryDesc = factoryDesc; }

    protected:
        AnimationAssetId m_id;
        Engine::FactoryDesc m_factoryDesc;
    };
}

#endif // ANIMATION_ASSET_DTOS_H
