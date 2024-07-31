#include "SpatialId.h"
#include <cassert>

using namespace Enigma::SceneGraph;

SpatialId::SpatialId(const std::string& name, const Frameworks::Rtti& rtti)
    : m_name(name), m_rtti(&rtti)
{
}

SpatialId::SpatialId(const std::vector<std::string>& tokens)
{
    assert(tokens.size() == 2);
    m_name = tokens[0];
    m_rtti = &Frameworks::Rtti::fromName(tokens[1]);
}

SpatialId::~SpatialId()
{
}

SpatialId::SpatialId(const SpatialId& other)
    : m_name(other.m_name), m_rtti(other.m_rtti)
{
}

SpatialId::SpatialId(SpatialId&& other) noexcept
{
    m_name = std::move(other.m_name);
    m_rtti = std::move(other.m_rtti);
}

SpatialId& SpatialId::operator=(const SpatialId& other)
{
    m_name = other.m_name;
    m_rtti = other.m_rtti;
    return *this;
}

SpatialId& SpatialId::operator=(SpatialId&& other) noexcept
{
    m_name = std::move(other.m_name);
    m_rtti = std::move(other.m_rtti);
    return *this;
}

std::vector<std::string> SpatialId::tokens() const
{
    return { m_name, m_rtti->getName() };
}
