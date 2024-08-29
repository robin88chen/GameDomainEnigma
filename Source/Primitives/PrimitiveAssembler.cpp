#include "PrimitiveAssembler.h"
#include "Primitive.h"

using namespace Enigma::Primitives;

PrimitiveAssembler::PrimitiveAssembler(const PrimitiveId& id)
    : m_id(id), m_factoryDesc(Primitive::TYPE_RTTI)
{
}

PrimitiveDisassembler::PrimitiveDisassembler()
    : m_factoryDesc(Primitive::TYPE_RTTI)
{
}