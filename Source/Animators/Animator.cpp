#include "Animator.h"

using namespace Enigma::Animators;

DEFINE_RTTI_OF_BASE(Animators, Animator);

Animator::Animator()
{

}

Animator::Animator(const Animator& ani)
{

}

Animator::Animator(Animator&& ani)
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
