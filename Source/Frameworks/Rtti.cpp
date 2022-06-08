#include "Rtti.h"
#include "RttiDerivingMap.h"
#include <cassert>

using namespace Enigma::Frameworks;

Rtti::Rtti(const std::string& name)
{
    m_name = name;
}

Rtti::Rtti(const std::string& name, const Rtti& base_rtti)
{
    bool is_mapped = RttiDerivingMap::TryInsertDeriving(name, base_rtti.GetName());
    assert(is_mapped);
    m_name = name;
}

bool Rtti::operator==(const Rtti& rhs) const
{
    return IsExactly(rhs);
}

bool Rtti::IsExactly(const Rtti& type) const
{
    return type.m_name == m_name;
}

bool Rtti::IsDerived(const Rtti& type) const
{
    return RttiDerivingMap::IsDerivedFrom(m_name, type.m_name);
}

const std::string& Rtti::GetName() const
{
    return m_name;
}
