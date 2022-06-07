#include "RttiRepository.h"
#include <cassert>

using namespace Enigma::Frameworks;

RttiRepository::RttiDerivingMap RttiRepository::m_derivingMap;

Rtti RttiRepository::CreateDerived(const std::string& type_token, const std::string& base_rtti_token)
{
    if (auto it = m_derivingMap.find(type_token); it != m_derivingMap.end())
    {
        assert(it->second == base_rtti_token);
    }
    else
    {
        m_derivingMap.insert({ type_token, base_rtti_token });
    }
    return Rtti(type_token);
}

Rtti RttiRepository::Create(const std::string& type_token)
{
    return Rtti(type_token);
}

bool RttiRepository::IsDerivedFrom(const std::string& type_token, const std::string& base_rtti_token)
{
    std::string search_type = type_token;
    auto it = m_derivingMap.find(search_type);
    while (it != m_derivingMap.end())
    {
        if (it->second == base_rtti_token) return true;
        search_type = it->second;
    }
    return false;
}
