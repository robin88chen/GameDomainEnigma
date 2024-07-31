#include "PrimitiveId.h"
#include "Frameworks/Rtti.h"
#include "PrimitiveQueries.h"
#include "Frameworks/QueryDispatcher.h"
#include <cassert>

using namespace  Enigma::Primitives;

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

PrimitiveId::PrimitiveId(const std::vector<std::string>& tokens)
{
    assert(tokens.size() == 3);
    m_name = tokens[0];
    m_sequence = std::stoull(tokens[1]);
    m_rtti = &Frameworks::Rtti::fromName(tokens[2]);
}

PrimitiveId::PrimitiveId(const PrimitiveId& other)
{
    m_name = other.m_name;
    m_sequence = other.m_sequence;
    m_rtti = other.m_rtti;
}

PrimitiveId::PrimitiveId(PrimitiveId&& other) noexcept
{
    m_name = std::move(other.m_name);
    m_sequence = std::move(other.m_sequence);
    m_rtti = std::move(other.m_rtti);
}

PrimitiveId& PrimitiveId::operator=(const PrimitiveId& other)
{
    m_name = other.m_name;
    m_sequence = other.m_sequence;
    m_rtti = other.m_rtti;
    return *this;
}

PrimitiveId& PrimitiveId::operator=(PrimitiveId&& other) noexcept
{
    m_name = std::move(other.m_name);
    m_sequence = std::move(other.m_sequence);
    m_rtti = std::move(other.m_rtti);
    return *this;
}

std::vector<std::string> PrimitiveId::tokens() const
{
    return { m_name, std::to_string(m_sequence), m_rtti->getName() };
}

PrimitiveId PrimitiveId::next() const
{
    const auto query = std::make_shared<QueryPrimitiveNextSequenceNumber>(*this);
    Frameworks::QueryDispatcher::dispatch(query);
    return PrimitiveId(m_name, query->getResult(), *m_rtti);
}
