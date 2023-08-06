#include "Animator.h"

using namespace Enigma::Engine;

DEFINE_RTTI_OF_BASE(Engine, Animator);

Animator::Animator() : m_isListened(false), m_factoryDesc(Animator::TYPE_RTTI.GetName())
{

}

Animator::Animator(const Animator& ani) : m_isListened(false), m_factoryDesc(ani.m_factoryDesc)
{

}

Animator::Animator(Animator&& ani) noexcept : m_isListened(ani.m_isListened), m_factoryDesc(ani.m_factoryDesc)
{

}

Animator::~Animator()
{

}

Animator& Animator::operator=(const Animator& ani)
{
    m_isListened = false;
    m_factoryDesc = ani.m_factoryDesc;
    return *this;
}

Animator& Animator::operator=(Animator&& ani) noexcept
{
    m_isListened = ani.m_isListened;
    m_factoryDesc = ani.m_factoryDesc;
    return *this;
}
