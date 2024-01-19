#include "AnimationAssetDtos.h"
#include "AnimationAsset.h"

using namespace Enigma::Animators;

AnimationAssetDto::AnimationAssetDto() : m_factoryDesc(AnimationAsset::TYPE_RTTI.getName())
{
}
