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
    m_registerFactory =
        std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { this->registerFactory(c); });
    CommandBus::subscribe(typeid(RegisterDtoFactory), m_registerFactory);
    m_unregisterFactory =
        std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { this->unregisterFactory(c); });
    CommandBus::subscribe(typeid(UnRegisterDtoFactory), m_unregisterFactory);
    m_invokeDtoFactory =
        std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { this->invokeDtoFactory(c); });
    CommandBus::subscribe(typeid(InvokeDtoFactory), m_invokeDtoFactory);
    m_registerConverter =
        std::make_shared<CommandSubscriber>([=](auto c) { this->registerConverter(c); });
    CommandBus::subscribe(typeid(RegisterDtoPolicyConverter), m_registerConverter);
    m_unregisterConverter =
        std::make_shared<CommandSubscriber>([=](auto c) { this->unregisterConverter(c); });
    CommandBus::subscribe(typeid(UnRegisterDtoPolicyConverter), m_unregisterConverter);
}

GenericDtoFactories::~GenericDtoFactories()
{
    CommandBus::unsubscribe(typeid(RegisterDtoFactory), m_registerFactory);
    m_registerFactory = nullptr;
    CommandBus::unsubscribe(typeid(UnRegisterDtoFactory), m_unregisterFactory);
    m_unregisterFactory = nullptr;
    CommandBus::unsubscribe(typeid(InvokeDtoFactory), m_invokeDtoFactory);
    m_invokeDtoFactory = nullptr;
    CommandBus::unsubscribe(typeid(RegisterDtoPolicyConverter), m_registerConverter);
    m_registerConverter = nullptr;
    CommandBus::unsubscribe(typeid(UnRegisterDtoPolicyConverter), m_unregisterConverter);
    m_unregisterConverter = nullptr;
}

void GenericDtoFactories::registerFactory(const std::string& rtti, const DtoFactory& factory)
{
    m_factories.insert_or_assign(rtti, factory);
}

void GenericDtoFactories::unregisterFactory(const std::string& rtti)
{
    if (m_factories.find(rtti) != m_factories.end()) m_factories.erase(rtti);
}

void GenericDtoFactories::invokeFactory(const GenericDto& dto)
{
    auto factory = m_factories.find(dto.getRtti().GetRttiName());
    if (factory == m_factories.end())
    {
        Debug::Printf("Can't find dto factory of %s", dto.getRtti().GetRttiName().c_str());
        return;
    }
    return factory->second(dto);
}

void GenericDtoFactories::registerFactory(const ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<RegisterDtoFactory, ICommand>(c);
    if (!cmd) return;
    registerFactory(cmd->getRtti(), cmd->GetFactory());
}

void GenericDtoFactories::unregisterFactory(const ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<UnRegisterDtoFactory, ICommand>(c);
    if (!cmd) return;
    unregisterFactory(cmd->getRtti());
}

void GenericDtoFactories::invokeDtoFactory(const ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<InvokeDtoFactory, ICommand>(c);
    if (!cmd) return;
    invokeFactory(cmd->GetDto());
}

void GenericDtoFactories::registerConverter(const ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<RegisterDtoPolicyConverter, ICommand>(c);
    if (!cmd) return;
    GenericDto::registerConverter(cmd->getRtti(), cmd->GetPolicyConverter());
}

void GenericDtoFactories::unregisterConverter(const ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<UnRegisterDtoPolicyConverter, ICommand>(c);
    if (!cmd) return;
    GenericDto::unregisterConverter(cmd->getRtti());
}
