#include "Rtti.h"

using namespace Enigma::Frameworks;

Rtti::Rtti(const std::string& name)
{
    m_name = name;
    m_base = nullptr;
}

Rtti::Rtti(const std::string& name, const Rtti* base_rtti)
{
    m_name = name;
    m_base = base_rtti;
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
