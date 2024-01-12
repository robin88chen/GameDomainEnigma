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
    CommandBus::subscribe(typeid(RegisterDtoFactory), m_doRegisteringFactory);
    m_doUnRegisteringFactory =
        std::make_shared<CommandSubscriber>([=](auto c) { this->DoUnRegisteringFactory(c); });
    CommandBus::subscribe(typeid(UnRegisterDtoFactory), m_doUnRegisteringFactory);
    m_doInvokingDtoFactory =
        std::make_shared<CommandSubscriber>([=](auto c) { this->DoInvokingDtoFactory(c); });
    CommandBus::subscribe(typeid(InvokeDtoFactory), m_doInvokingDtoFactory);
    m_doRegisteringConverter =
        std::make_shared<CommandSubscriber>([=](auto c) { this->DoRegisteringConverter(c); });
    CommandBus::subscribe(typeid(RegisterDtoPolicyConverter), m_doRegisteringConverter);
    m_doUnRegisteringConverter =
        std::make_shared<CommandSubscriber>([=](auto c) { this->DoUnRegisteringConverter(c); });
    CommandBus::subscribe(typeid(UnRegisterDtoPolicyConverter), m_doUnRegisteringConverter);
}

GenericDtoFactories::~GenericDtoFactories()
{
    CommandBus::unsubscribe(typeid(RegisterDtoFactory), m_doRegisteringFactory);
    m_doRegisteringFactory = nullptr;
    CommandBus::unsubscribe(typeid(UnRegisterDtoFactory), m_doUnRegisteringFactory);
    m_doUnRegisteringFactory = nullptr;
    CommandBus::unsubscribe(typeid(InvokeDtoFactory), m_doInvokingDtoFactory);
    m_doInvokingDtoFactory = nullptr;
    CommandBus::unsubscribe(typeid(RegisterDtoPolicyConverter), m_doRegisteringConverter);
    m_doRegisteringConverter = nullptr;
    CommandBus::unsubscribe(typeid(UnRegisterDtoPolicyConverter), m_doUnRegisteringConverter);
    m_doUnRegisteringConverter = nullptr;
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
    auto factory = m_factories.find(dto.getRtti().GetRttiName());
    if (factory == m_factories.end())
    {
        Debug::Printf("Can't find dto factory of %s", dto.getRtti().GetRttiName().c_str());
        return;
    }
    return factory->second(dto);
}

void GenericDtoFactories::DoRegisteringFactory(const ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<RegisterDtoFactory, ICommand>(c);
    if (!cmd) return;
    RegisterFactory(cmd->getRtti(), cmd->GetFactory());
}

void GenericDtoFactories::DoUnRegisteringFactory(const ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<UnRegisterDtoFactory, ICommand>(c);
    if (!cmd) return;
    UnregisterFactory(cmd->getRtti());
}

void GenericDtoFactories::DoInvokingDtoFactory(const ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<InvokeDtoFactory, ICommand>(c);
    if (!cmd) return;
    InvokeFactory(cmd->GetDto());
}

void GenericDtoFactories::DoRegisteringConverter(const ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<RegisterDtoPolicyConverter, ICommand>(c);
    if (!cmd) return;
    GenericDto::RegisterConverter(cmd->getRtti(), cmd->GetPolicyConverter());
}

void GenericDtoFactories::DoUnRegisteringConverter(const ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<UnRegisterDtoPolicyConverter, ICommand>(c);
    if (!cmd) return;
    GenericDto::UnregisterConverter(cmd->getRtti());
}
