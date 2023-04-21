#include "Rtti.h"

using namespace Enigma::Frameworks;

std::unique_ptr<std::unordered_map<std::string, const Rtti*>> Rtti::m_valueMap;

Rtti::Rtti(const std::string& name)
{
    m_name = name;
    m_base = nullptr;
    if (!m_valueMap) m_valueMap = std::make_unique<std::unordered_map<std::string, const Rtti*>>();
    m_valueMap->insert_or_assign(name, this);
}

Rtti::Rtti(const std::string& name, const Rtti* base_rtti)
{
    m_name = name;
    m_base = base_rtti;
    if (!m_valueMap) m_valueMap = std::make_unique<std::unordered_map<std::string, const Rtti*>>();
    m_valueMap->insert_or_assign(name, this);
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
    auto iter_type = m_valueMap->find(type_token);
    auto iter_base = m_valueMap->find(base_rtti_token);
    if (iter_type == m_valueMap->end() || iter_base == m_valueMap->end()) return false;
    return (*iter_type->second).IsDerived(*iter_base->second);
}

bool Rtti::IsExactlyOrDerivedFrom(const std::string& type_token, const std::string& base_rtti_token)
{
    return type_token == base_rtti_token || IsDerivedFrom(type_token, base_rtti_token);
}

