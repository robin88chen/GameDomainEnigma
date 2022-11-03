#include "ContractedFactories.h"
#include "Contract.h"
#include "Platforms/PlatformLayer.h"
#include "Frameworks/CommandBus.h"
#include "ContractCommands.h"

using namespace Enigma::Engine;
using namespace Enigma::Platforms;
using namespace Enigma::Frameworks;

DEFINE_RTTI(Engine, ContractedFactories, ISystemService);

ContractedFactories::ContractedFactories(ServiceManager* srv_mngr) : ISystemService(srv_mngr)
{
    m_doRegisteringFactory =
        std::make_shared<CommandSubscriber>([=](auto c) { this->DoRegisteringFactory(c); });
    CommandBus::Subscribe(typeid(RegisterContractFactory), m_doRegisteringFactory);
    m_doUnRegisteringFactory =
        std::make_shared<CommandSubscriber>([=](auto c) { this->DoUnRegisteringFactory(c); });
    CommandBus::Subscribe(typeid(UnRegisterContractFactory), m_doUnRegisteringFactory);
    m_doInvokingContractFactory =
        std::make_shared<CommandSubscriber>([=](auto c) { this->DoInvokingContractFactory(c); });
    CommandBus::Subscribe(typeid(InvokeContractFactory), m_doInvokingContractFactory);
}

ContractedFactories::~ContractedFactories()
{
    CommandBus::Unsubscribe(typeid(RegisterContractFactory), m_doRegisteringFactory);
    m_doRegisteringFactory = nullptr;
    CommandBus::Unsubscribe(typeid(UnRegisterContractFactory), m_doUnRegisteringFactory);
    m_doUnRegisteringFactory = nullptr;
    CommandBus::Unsubscribe(typeid(InvokeContractFactory), m_doInvokingContractFactory);
    m_doInvokingContractFactory = nullptr;
}

void ContractedFactories::RegisterFactory(const std::string& rtti, const ContractFactory& factory)
{
    m_factories.insert_or_assign(rtti, factory);
}

void ContractedFactories::UnregisterFactory(const std::string& rtti)
{
    if (m_factories.find(rtti) != m_factories.end()) m_factories.erase(rtti);
}

void ContractedFactories::InvokeFactory(const Contract& contract)
{
    auto factory = m_factories.find(contract.GetRtti().GetRttiName());
    if (factory == m_factories.end())
    {
        Debug::Printf("Can't find contract factory of %s", contract.GetRtti().GetRttiName().c_str());
        return;
    }
    return factory->second(contract);
}

void ContractedFactories::DoRegisteringFactory(const ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<RegisterContractFactory, ICommand>(c);
    if (!cmd) return;
    RegisterFactory(cmd->GetRtti(), cmd->GetFactory());
}

void ContractedFactories::DoUnRegisteringFactory(const ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<UnRegisterContractFactory, ICommand>(c);
    if (!cmd) return;
    UnregisterFactory(cmd->GetRtti());
}

void ContractedFactories::DoInvokingContractFactory(const ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<InvokeContractFactory, ICommand>(c);
    if (!cmd) return;
    InvokeFactory(cmd->GetContract());
}

