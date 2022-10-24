/*********************************************************************
 * \file   ContractedFactories.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2022
 *********************************************************************/
#ifndef CONTRACTED_FACTORIES_H
#define CONTRACTED_FACTORIES_H

#include <functional>
#include <unordered_map>
#include <any>

namespace Enigma::Frameworks
{
    class Contract;

    using ValueContractFactory = std::function<std::any(const Contract& contract)>;

    class ContractedFactories
    {
    public:
        static void RegisterFactory(const std::string& rtti, const ValueContractFactory& factory);
        static void UnregisterFactory(const std::string& rtti);

        static std::any InvokeFactory(const Contract& contract);
        template<class T> static T InvokeFactory(const Contract& contract)
        {
            return std::any_cast<T>(InvokeFactory(contract));
        }
    private:
        using ValueContractFactoryTable = std::unordered_map<std::string, ValueContractFactory>;
        static ValueContractFactoryTable m_valueFactories;
    };
}

#endif // CONTRACTED_FACTORIES_H
