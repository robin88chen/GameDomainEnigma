#include "AnimationAsset.h"
#include "AnimationAssetQueries.h"
#include "Frameworks/QueryDispatcher.h"

using namespace Enigma::Animators;

DEFINE_RTTI_OF_BASE(Animators, AnimationAsset);

AnimationAsset::AnimationAsset(const AnimationAssetId& id) : m_factoryDesc(AnimationAsset::TYPE_RTTI.getName())
{
    m_id = id;
}

AnimationAsset::~AnimationAsset()
{

}

std::shared_ptr<AnimationAsset> AnimationAsset::queryAnimationAsset(const AnimationAssetId& id)
{
    const auto query = std::make_shared<QueryAnimationAsset>(id);
    Frameworks::QueryDispatcher::dispatch(query);
    return query->getResult();
}
