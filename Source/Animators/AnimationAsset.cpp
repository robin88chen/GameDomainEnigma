#include "AnimationAsset.h"

using namespace Enigma::Animators;

DEFINE_RTTI_OF_BASE(Animators, AnimationAsset);

AnimationAsset::AnimationAsset(const std::string& name)
{
    m_name = name;
}

AnimationAsset::~AnimationAsset()
{

}
