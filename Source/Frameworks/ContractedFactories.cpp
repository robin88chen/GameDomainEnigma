#include "ContractedFactories.h"
#include "Contract.h"
#include "Platforms/PlatformLayer.h"

using namespace Enigma::Frameworks;
using namespace Enigma::Platforms;

void ContractedFactories::RegisterFactory(const std::string& rtti, const ValueContractFactory& factory)
{
    m_valueFactories.insert_or_assign(rtti, factory);
}

void ContractedFactories::UnregisterFactory(const std::string& rtti)
{
    m_valueFactories.erase(rtti);
}

std::any ContractedFactories::InvokeFactory(const Contract& contract)
{
    auto factory = m_valueFactories.find(contract.GetRtti().GetRttiName());
    if (factory == m_valueFactories.end())
    {
        Debug::Printf("Can't find contract factory of %s", contract.GetRtti().GetRttiName().c_str());
        return std::any();
    }
    return factory->second(contract);
}
