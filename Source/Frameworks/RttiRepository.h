/*********************************************************************
 * \file   RttiRepository.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   June 2022
 *********************************************************************/
#ifndef RTTI_REPOSITORY_H
#define RTTI_REPOSITORY_H

#include "Rtti.h"
#include <unordered_map>

namespace Enigma::Frameworks
{
    class RttiRepository
    {
    public:
        static Rtti CreateDerived(const std::string& type_token, const std::string& base_rtti_token);
        static Rtti Create(const std::string& type_token);

    private:
        friend class Rtti;
        static bool IsDerivedFrom(const std::string& type_token, const std::string& base_rtti_token);

    private:
        using RttiDerivingMap = std::unordered_map<std::string, std::string>;
        static RttiDerivingMap m_derivingMap;
    };
}

#endif // RTTI_REPOSITORY_H
