#include "EffectMaterialId.h"
#include <cassert>

using namespace Enigma::Engine;

EffectMaterialId::EffectMaterialId(const std::string& name) : m_name(name), m_instance_serial(0)
{
}

EffectMaterialId::EffectMaterialId(const EffectMaterialId& source, std::uint32_t instance_serial)
{
    assert(source.isSource());
    m_name = source.m_name;
    m_instance_serial = instance_serial;
}

EffectMaterialId::EffectMaterialId(const EffectMaterialId& other)
{
    m_name = other.m_name;
    m_instance_serial = other.m_instance_serial;
}

EffectMaterialId::EffectMaterialId(EffectMaterialId&& other) noexcept
{
    m_name = std::move(other.m_name);
    m_instance_serial = other.m_instance_serial;
}

EffectMaterialId& EffectMaterialId::operator=(const EffectMaterialId& other)
{
    m_name = other.m_name;
    m_instance_serial = other.m_instance_serial;
    return *this;
}

EffectMaterialId& EffectMaterialId::operator=(EffectMaterialId&& other) noexcept
{
    m_name = std::move(other.m_name);
    m_instance_serial = other.m_instance_serial;
    return *this;
}

bool EffectMaterialId::operator==(const EffectMaterialId& other) const
{
    return m_name == other.m_name && m_instance_serial == other.m_instance_serial;
}

bool EffectMaterialId::operator!=(const EffectMaterialId& other) const
{
    return m_name != other.m_name || m_instance_serial != other.m_instance_serial;
}
