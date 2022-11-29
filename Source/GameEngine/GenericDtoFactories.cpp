#include "GenericDtoFactories.h"
#include "GenericDto.h"
#include "Platforms/PlatformLayer.h"
#include "Frameworks/CommandBus.h"
#include "FactoryCommands.h"

using namespace Enigma::Engine;
using namespace Enigma::Platforms;
using namespace Enigma::Frameworks;

DEFINE_RTTI(Engine, GenericDtoFactories, ISystemService);

GenericDtoFactories::GenericDtoFactories(ServiceManager* srv_mngr) : ISystemService(srv_mngr)
{
    m_doRegisteringFactory =
        std::make_shared<CommandSubscriber>([=](auto c) { this->DoRegisteringFactory(c); });
    CommandBus::Subscribe(typeid(RegisterDtoFactory), m_doRegisteringFactory);
    m_doUnRegisteringFactory =
        std::make_shared<CommandSubscriber>([=](auto c) { this->DoUnRegisteringFactory(c); });
    CommandBus::Subscribe(typeid(UnRegisterDtoFactory), m_doUnRegisteringFactory);
    m_doInvokingDtoFactory =
        std::make_shared<CommandSubscriber>([=](auto c) { this->DoInvokingDtoFactory(c); });
    CommandBus::Subscribe(typeid(InvokeDtoFactory), m_doInvokingDtoFactory);
}

GenericDtoFactories::~GenericDtoFactories()
{
    CommandBus::Unsubscribe(typeid(RegisterDtoFactory), m_doRegisteringFactory);
    m_doRegisteringFactory = nullptr;
    CommandBus::Unsubscribe(typeid(UnRegisterDtoFactory), m_doUnRegisteringFactory);
    m_doUnRegisteringFactory = nullptr;
    CommandBus::Unsubscribe(typeid(InvokeDtoFactory), m_doInvokingDtoFactory);
    m_doInvokingDtoFactory = nullptr;
}

void GenericDtoFactories::RegisterFactory(const std::string& rtti, const DtoFactory& factory)
{
    m_factories.insert_or_assign(rtti, factory);
}

void GenericDtoFactories::UnregisterFactory(const std::string& rtti)
{
    if (m_factories.find(rtti) != m_factories.end()) m_factories.erase(rtti);
}

void GenericDtoFactories::InvokeFactory(const GenericDto& dto)
{
    auto factory = m_factories.find(dto.GetRtti().GetRttiName());
    if (factory == m_factories.end())
    {
        Debug::Printf("Can't find dto factory of %s", dto.GetRtti().GetRttiName().c_str());
        return;
    }
    return factory->second(dto);
}

void GenericDtoFactories::DoRegisteringFactory(const ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<RegisterDtoFactory, ICommand>(c);
    if (!cmd) return;
    RegisterFactory(cmd->GetRtti(), cmd->GetFactory());
}

void GenericDtoFactories::DoUnRegisteringFactory(const ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<UnRegisterDtoFactory, ICommand>(c);
    if (!cmd) return;
    UnregisterFactory(cmd->GetRtti());
}

void GenericDtoFactories::DoInvokingDtoFactory(const ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<InvokeDtoFactory, ICommand>(c);
    if (!cmd) return;
    InvokeFactory(cmd->GetDto());
}

