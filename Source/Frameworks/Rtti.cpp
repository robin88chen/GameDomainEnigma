#include "Rtti.h"
#include "RttiDerivingMap.h"

using namespace Enigma::Frameworks;

RttiDerivingMap::DerivingMap RttiDerivingMap::m_derivingMap;  // 在這裡建立，應該就比 Rtti早

Rtti::Rtti(const std::string& name)
{
    m_name = name;
    m_base = nullptr;
}

Rtti::Rtti(const std::string& name, const Rtti* base_rtti)
{
    m_name = name;
    m_base = base_rtti;
    if (base_rtti) RttiDerivingMap::TryInsertDeriving(name, base_rtti->GetName());
}

bool Rtti::operator==(const Rtti& rhs) const
{
    return IsExactly(rhs);
}

bool Rtti::IsExactly(const Rtti& type) const
{
    return &type == this;
}

bool Rtti::IsDerived(const Rtti& type) const
{
    const Rtti* search = this;
    while (search)
    {
        if (search == &type)
            return true;
        search = search->m_base;
    }
    return false;
}

const std::string& Rtti::GetName() const
{
    return m_name;
}

bool Rtti::IsDerivedFrom(const std::string& type_token, const std::string& base_rtti_token)
{
    return RttiDerivingMap::IsDerivedFrom(type_token, base_rtti_token);
}

bool Rtti::IsExactlyOrDerivedFrom(const std::string& type_token, const std::string& base_rtti_token)
{
    return RttiDerivingMap::IsExactlyOrDerivedFrom(type_token, base_rtti_token);
}

