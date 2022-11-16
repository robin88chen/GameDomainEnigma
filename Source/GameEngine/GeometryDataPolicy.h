/*********************************************************************
 * \file   GeometryDataPolicy.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   November 2022
 *********************************************************************/
#ifndef _GEOMETRY_DATA_POLICY_H
#define _GEOMETRY_DATA_POLICY_H

#include "GeometryDataContract.h"
#include "ContractDeserializer.h"
#include <string>
#include <optional>

namespace Enigma::Engine
{
    class GeometryDataPolicy
    {
    public:
        GeometryDataPolicy() = default;
        GeometryDataPolicy(const std::string& name, const Contract& contract);
        GeometryDataPolicy(const std::string& name, const std::string& file_at_path, const ContractDeserializer& deserializer);

        [[nodiscard]] const std::string& Name() const { return m_name; }
        std::string& Name() { return m_name; }

        [[nodiscard]] const std::optional<Contract>& GetContract() const { return m_contract; }

        [[nodiscard]] const ContractDeserializer& GetDeserializer() const { return m_deserializer; }

    protected:
        std::string m_name;
        std::optional<Contract> m_contract;
        std::string m_fileAtPath;
        ContractDeserializer m_deserializer;
    };
}

#endif // _GEOMETRY_DATA_POLICY_H
