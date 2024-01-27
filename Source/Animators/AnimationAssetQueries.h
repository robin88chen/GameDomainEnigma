/*********************************************************************
 * \file   AnimationAssetQueries.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   January 2024
 *********************************************************************/
#ifndef ANIMATION_ASSET_QUERIES_H
#define ANIMATION_ASSET_QUERIES_H

#include "AnimationAssetId.h"
#include "Frameworks/Query.h"
namespace Enigma::Animators
{
    class AnimationAsset;

    class QueryAnimationAsset : public Frameworks::Query<std::shared_ptr<AnimationAsset>>
    {
    public:
        QueryAnimationAsset(const AnimationAssetId& id) : m_id(id) {}

        const AnimationAssetId& id() const { return m_id; }

    protected:
        AnimationAssetId m_id;
    };
}

#endif // ANIMATION_ASSET_QUERIES_H
