#include "RttiDerivingMap.h"
#include <cassert>

using namespace Enigma::Frameworks;

RttiDerivingMap::DerivingMap RttiDerivingMap::m_derivingMap;

bool RttiDerivingMap::TryInsertDeriving(const std::string& type_token, const std::string& base_rtti_token)
{
    if (auto iter = m_derivingMap.find(type_token); iter != m_derivingMap.end())
    {
        if (iter->second != base_rtti_token) return false;
    }
    else
    {
        m_derivingMap.insert({ type_token, base_rtti_token });
    }
    return true;
}

bool RttiDerivingMap::IsDerivedFrom(const std::string& type_token, const std::string& base_rtti_token)
{
    std::string search_type = type_token;
    auto iter = m_derivingMap.find(search_type);
    while (iter != m_derivingMap.end())
    {
        if (iter->second == base_rtti_token) return true;
        search_type = iter->second;
    }
    return false;
}
