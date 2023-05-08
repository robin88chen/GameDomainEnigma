#include "Animator.h"

using namespace Enigma::Engine;

DEFINE_RTTI_OF_BASE(Engine, Animator);

Animator::Animator() : m_isListened(false)
{

}

Animator::Animator(const Animator& ani) : m_isListened(ani.m_isListened)
{

}

Animator::Animator(Animator&& ani) noexcept : m_isListened(ani.m_isListened)
{

}

Animator::~Animator()
{

}

Animator& Animator::operator=(const Animator&)
{
    return *this;
}

Animator& Animator::operator=(Animator&&) noexcept
{
    return *this;
}
