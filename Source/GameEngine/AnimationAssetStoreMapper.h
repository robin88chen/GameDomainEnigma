/*********************************************************************
 * \file   AnimationAssetStoreMapper.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   January 2024
 *********************************************************************/
#ifndef ANIMATION_ASSET_STORE_MAPPER_H
#define ANIMATION_ASSET_STORE_MAPPER_H

#include "GenericDto.h"
#include "AnimationAssetId.h"
#include <system_error>

namespace Enigma::Engine
{
    class AnimationAssetStoreMapper
    {
    public:
        virtual ~AnimationAssetStoreMapper() = default;

        virtual std::error_code connect() = 0;
        virtual std::error_code disconnect() = 0;

        virtual bool hasAnimationAsset(const AnimationAssetId& id) = 0;
        virtual std::optional<GenericDto> queryAnimationAsset(const AnimationAssetId& id) = 0;
        virtual std::error_code removeAnimationAsset(const AnimationAssetId& id) = 0;
        virtual std::error_code putAnimationAsset(const AnimationAssetId& id, const GenericDto& dto) = 0;
    };
}

#endif // ANIMATION_ASSET_STORE_MAPPER_H
