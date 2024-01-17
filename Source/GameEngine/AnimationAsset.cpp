#include "AnimationAsset.h"

using namespace Enigma::Engine;

DEFINE_RTTI_OF_BASE(Engine, AnimationAsset);

AnimationAsset::AnimationAsset(const std::string& name) : m_factoryDesc(AnimationAsset::TYPE_RTTI.getName())
{
    m_name = name;
}

AnimationAsset::~AnimationAsset()
{

}
