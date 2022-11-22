#include "Primitive.h"

using namespace Enigma::Engine;

DEFINE_RTTI_OF_BASE(Engine, Primitive);

Primitive::Primitive()
{
    m_mxPrimitiveWorld = MathLib::Matrix4::IDENTITY;
    m_primitiveFlags = 0;
}

Primitive::~Primitive()
{
}
