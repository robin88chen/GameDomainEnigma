#include "AnimatorAssembler.h"
#include "Animator.h"

using namespace Enigma::Animators;

AnimatorAssembler::AnimatorAssembler(const AnimatorId& id) : m_id(id), m_factoryDesc(Animator::TYPE_RTTI)
{
}

AnimatorDisassembler::AnimatorDisassembler() : m_factoryDesc(Animator::TYPE_RTTI)
{
}
