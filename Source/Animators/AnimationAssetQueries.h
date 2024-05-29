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
#include "Frameworks/Rtti.h"

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
    class RequestAnimationAssetCreation : public Frameworks::Query<std::shared_ptr<AnimationAsset>>
    {
    public:
        RequestAnimationAssetCreation(const AnimationAssetId& id, const Frameworks::Rtti& rtti) : m_id(id), m_rtti(rtti.getName()) {}
        const AnimationAssetId& id() { return m_id; }
        const Frameworks::Rtti& rtti() { return Frameworks::Rtti::fromName(m_rtti); }

    private:
        AnimationAssetId m_id;
        std::string m_rtti;
    };
    class RequestAnimationAssetConstitution : public Frameworks::Query<std::shared_ptr<AnimationAsset>>
    {
    public:
        RequestAnimationAssetConstitution(const AnimationAssetId& id, const Engine::GenericDto& dto) : m_id(id), m_dto(dto) {}
        const AnimationAssetId& id() { return m_id; }
        const Engine::GenericDto& dto() { return m_dto; }

    private:
        AnimationAssetId m_id;
        Engine::GenericDto m_dto;
    };
}

#endif // ANIMATION_ASSET_QUERIES_H
