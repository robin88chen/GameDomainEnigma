#include "Animator.h"

using namespace Enigma::Animators;

DEFINE_RTTI_OF_BASE(Animators, Animator);

Animator::Animator() : m_isListened(false)
{

}

Animator::Animator(const Animator& ani) : m_isListened(false)
{

}

Animator::Animator(Animator&& ani) : m_isListened(false)
{

}

Animator::~Animator()
{

}

Animator& Animator::operator=(const Animator&)
{
    return *this;
}

Animator& Animator::operator=(Animator&&)
{
    return *this;
}
