/*********************************************************************
 * \file   RttiDerivingMap.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   June 2022
 *********************************************************************/
#ifndef RTTI_DERIVING_MAP_H
#define RTTI_DERIVING_MAP_H

#include "Rtti.h"
#include <unordered_map>

namespace Enigma::Frameworks
{
    class RttiDerivingMap
    {
    private:
        friend class Rtti;
        static bool TryInsertDeriving(const std::string& type_token, const std::string& base_rtti_token);
        static bool IsDerivedFrom(const std::string& type_token, const std::string& base_rtti_token);

    private:
        using DerivingMap = std::unordered_map<std::string, std::string>;
        static DerivingMap m_derivingMap;
    };
}

#endif // RTTI_DERIVING_MAP_H
