#include "Primitive.h"

using namespace Enigma::Engine;

DEFINE_RTTI_OF_BASE(Engine, Primitive);

Primitive::Primitive() : m_factoryDesc(Primitive::TYPE_RTTI.getName())
{
    m_mxPrimitiveWorld = MathLib::Matrix4::IDENTITY;
    m_primitiveFlags = 0;
}

Primitive::~Primitive()
{
}

void Primitive::EnumAnimatorListDeep(std::list<std::shared_ptr<Animator>>& resultList)
{
    if (m_animator) resultList.push_back(m_animator);
}
