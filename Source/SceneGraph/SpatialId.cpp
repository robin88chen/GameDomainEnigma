#include "SpatialId.h"

using namespace Enigma::SceneGraph;

SpatialId::SpatialId(const std::string& name, const Frameworks::Rtti& rtti)
    : m_name(name), m_rtti(&rtti)
{
}

SpatialId::~SpatialId()
{
}

SpatialId::SpatialId(const SpatialId& other)
    : m_name(other.m_name), m_rtti(other.m_rtti)
{
}

SpatialId::SpatialId(SpatialId&& other)
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

SpatialId& SpatialId::operator=(SpatialId&& other)
{
    m_name = std::move(other.m_name);
    m_rtti = std::move(other.m_rtti);
    return *this;
}
