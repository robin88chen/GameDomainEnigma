#include "PrimitiveId.h"
#include "Frameworks/Rtti.h"

using namespace  Enigma::Engine;

PrimitiveId::PrimitiveId(const std::string& name, const Frameworks::Rtti& rtti)
{
    m_name = name;
    m_sequence = 0;
    m_rtti = &rtti;
}

PrimitiveId::PrimitiveId(const std::string& name, std::uint64_t sequence, const Frameworks::Rtti& rtti)
{
    m_name = name;
    m_sequence = sequence;
    m_rtti = &rtti;
}

PrimitiveId::PrimitiveId(const PrimitiveId& other)
{
    m_name = other.m_name;
    m_sequence = other.m_sequence;
    m_rtti = other.m_rtti;
}

PrimitiveId::PrimitiveId(PrimitiveId&& other)
{
    m_name = std::move(other.m_name);
    m_sequence = std::move(other.m_sequence);
    m_rtti = std::move(other.m_rtti);
}

PrimitiveId::~PrimitiveId()
{
}

PrimitiveId& PrimitiveId::operator=(const PrimitiveId& other)
{
    m_name = other.m_name;
    m_sequence = other.m_sequence;
    m_rtti = other.m_rtti;
    return *this;
}

PrimitiveId& PrimitiveId::operator=(PrimitiveId&& other)
{
    m_name = std::move(other.m_name);
    m_sequence = std::move(other.m_sequence);
    m_rtti = std::move(other.m_rtti);
    return *this;
}
