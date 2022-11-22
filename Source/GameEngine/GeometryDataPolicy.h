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
        GeometryDataPolicy(const std::string& name, const Contract& contract) : m_name(name), m_contract(contract) {}
        GeometryDataPolicy(const std::string& name, const std::string& deserialize_param,
            const std::shared_ptr<IContractDeserializer>& deserializer) : m_name(name), m_parameter(deserialize_param), m_deserializer(deserializer) {}

        [[nodiscard]] const std::string& Name() const { return m_name; }
        std::string& Name() { return m_name; }

        [[nodiscard]] const std::optional<Contract>& GetContract() const { return m_contract; }

        [[nodiscard]] const std::string& Parameter() const { return m_parameter; }
        [[nodiscard]] const std::shared_ptr<IContractDeserializer>& GetDeserializer() const { return m_deserializer; }

    protected:
        std::string m_name;
        std::optional<Contract> m_contract;
        std::string m_parameter;
        std::shared_ptr<IContractDeserializer> m_deserializer;
    };
}

#endif // _GEOMETRY_DATA_POLICY_H
