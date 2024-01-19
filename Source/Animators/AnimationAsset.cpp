#include "AnimationAsset.h"

using namespace Enigma::Animators;

DEFINE_RTTI_OF_BASE(Animators, AnimationAsset);

AnimationAsset::AnimationAsset(const AnimationAssetId& id) : m_factoryDesc(AnimationAsset::TYPE_RTTI.getName())
{
    m_id = id;
}

AnimationAsset::~AnimationAsset()
{

}
