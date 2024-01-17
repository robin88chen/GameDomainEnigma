#include "AnimatorId.h"
#include "AnimatorQueries.h"
#include "Frameworks/QueryDispatcher.h"
#include <cassert>

Enigma::Engine::AnimatorId::AnimatorId(const std::string& name, const Frameworks::Rtti& rtti)
{
    m_name = name;
    m_sequence = 0;
    m_rtti = &rtti;
}

Enigma::Engine::AnimatorId::AnimatorId(const std::string& name, std::uint64_t sequence, const Frameworks::Rtti& rtti)
{
    m_name = name;
    m_sequence = sequence;
    m_rtti = &rtti;
}

Enigma::Engine::AnimatorId::AnimatorId(const std::vector<std::string>& tokens)
{
    assert(tokens.size() == 3);
    m_name = tokens[0];
    m_sequence = std::stoull(tokens[1]);
    m_rtti = &Frameworks::Rtti::fromName(tokens[2]);
}

Enigma::Engine::AnimatorId::AnimatorId(const AnimatorId& other)
{
    m_name = other.m_name;
    m_sequence = other.m_sequence;
    m_rtti = other.m_rtti;
}

Enigma::Engine::AnimatorId& Enigma::Engine::AnimatorId::operator=(const AnimatorId& other)
{
    m_name = other.m_name;
    m_sequence = other.m_sequence;
    m_rtti = other.m_rtti;
    return *this;
}

Enigma::Engine::AnimatorId::AnimatorId(AnimatorId&& other) noexcept
{
    m_name = std::move(other.m_name);
    m_sequence = other.m_sequence;
    m_rtti = other.m_rtti;
}

Enigma::Engine::AnimatorId& Enigma::Engine::AnimatorId::operator=(AnimatorId&& other) noexcept
{
    m_name = std::move(other.m_name);
    m_sequence = other.m_sequence;
    m_rtti = other.m_rtti;
    return *this;
}

std::vector<std::string> Enigma::Engine::AnimatorId::tokens() const
{
    return { m_name, std::to_string(m_sequence), m_rtti->getName() };
}

Enigma::Engine::AnimatorId Enigma::Engine::AnimatorId::next() const
{
    const auto query = std::make_shared<QueryAnimatorNextSequenceNumber>(*this);
    Frameworks::QueryDispatcher::dispatch(query);
    return AnimatorId{ m_name, query->getResult(), *m_rtti };
}
