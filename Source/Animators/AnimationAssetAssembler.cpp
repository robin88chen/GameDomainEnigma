#include "AnimationAssetAssembler.h"
#include "AnimationAsset.h"

using namespace Enigma::Animators;

AnimationAssetAssembler::AnimationAssetAssembler(const AnimationAssetId& id) : m_id(id), m_factoryDesc(AnimationAsset::TYPE_RTTI.getName())
{
}

AnimationAssetDisassembler::AnimationAssetDisassembler() : m_factoryDesc(Engine::FactoryDesc(AnimationAsset::TYPE_RTTI.getName()))
{
}
