#include "GeometryBuilder.h"
#include "GeometryDataEvents.h"
#include "GeometryRepository.h"
#include "GeometryDataPolicy.h"
#include "EngineErrors.h"
#include "Frameworks/EventPublisher.h"
#include "Frameworks/EventSubscriber.h"
#include "Frameworks/CommandBus.h"
#include "DtoDeserializer.h"
#include "DtoEvents.h"
#include "FactoryCommands.h"
#include "Platforms/PlatformLayer.h"
#include <cassert>
#include <memory>

using namespace Enigma::Engine;
using namespace Enigma::Frameworks;

GeometryBuilder::GeometryBuilder(GeometryRepository* host) : m_ruidDeserializing(), m_ruidInstancing()
{
    m_hostRepository = host;
    m_onDtoDeserialized = std::make_shared<EventSubscriber>([=](auto e) { this->OnDtoDeserialized(e); });
    EventPublisher::Subscribe(typeid(GenericDtoDeserialized), m_onDtoDeserialized);
    m_onDeserializeDtoFailed = std::make_shared<EventSubscriber>([=](auto e) { this->OnDeserializeDtoFailed(e); });
    EventPublisher::Subscribe(typeid(DeserializeDtoFailed), m_onDeserializeDtoFailed);
    m_onFactoryGeometryCreated = std::make_shared<EventSubscriber>([=](auto e) { this->OnFactoryGeometryCreated(e); });
    EventPublisher::Subscribe(typeid(FactoryGeometryCreated), m_onFactoryGeometryCreated);

    m_doRegisteringGeometryFactory =
        std::make_shared<CommandSubscriber>([=](auto c) { this->DoRegisteringGeometryFactory(c); });
    CommandBus::Subscribe(typeid(RegisterGeometryDtoFactory), m_doRegisteringGeometryFactory);
    m_doUnRegisteringGeometryFactory =
        std::make_shared<CommandSubscriber>([=](auto c) { this->DoUnRegisteringGeometryFactory(c); });
    CommandBus::Subscribe(typeid(UnRegisterGeometryDtoFactory), m_doUnRegisteringGeometryFactory);
}

GeometryBuilder::~GeometryBuilder()
{
    EventPublisher::Unsubscribe(typeid(GenericDtoDeserialized), m_onDtoDeserialized);
    m_onDtoDeserialized = nullptr;
    EventPublisher::Unsubscribe(typeid(DeserializeDtoFailed), m_onDeserializeDtoFailed);
    m_onDeserializeDtoFailed = nullptr;
    EventPublisher::Unsubscribe(typeid(FactoryGeometryCreated), m_onFactoryGeometryCreated);
    m_onFactoryGeometryCreated = nullptr;
    CommandBus::Unsubscribe(typeid(RegisterGeometryDtoFactory), m_doRegisteringGeometryFactory);
    m_doRegisteringGeometryFactory = nullptr;
    CommandBus::Unsubscribe(typeid(UnRegisterGeometryDtoFactory), m_doUnRegisteringGeometryFactory);
    m_doUnRegisteringGeometryFactory = nullptr;
}

void GeometryBuilder::BuildGeometry(const GeometryDataPolicy& policy)
{
    assert(m_hostRepository);
    m_policy = policy;
    if (m_hostRepository->HasGeometryData(policy.Name()))
    {
        EventPublisher::Post(std::make_shared<GeometryDataBuilt>(policy.Name(),
            m_hostRepository->QueryGeometryData(policy.Name())));
    }
    else if (auto p = policy.GetDto())
    {
        CreateFromDto(p.value());
    }
    else if (policy.GetDeserializer())
    {
        m_ruidDeserializing = Ruid::Generate();
        policy.GetDeserializer()->InvokeDeserialize(m_ruidDeserializing, policy.Parameter());
    }
    else
    {
        EventPublisher::Post(std::make_shared<BuildGeometryDataFailed>(policy.Name(), ErrorCode::policyIncomplete));
    }
}

void GeometryBuilder::CreateFromDto(const GenericDto& dto)
{
    assert(m_hostRepository);
    m_ruidInstancing = dto.GetId();
    GeometryFactory(dto);
    //CommandBus::Post(std::make_shared<InvokeDtoFactory>(dto));
}

void GeometryBuilder::GeometryFactory(const Engine::GenericDto& dto)
{
    auto factory = m_factories.find(dto.GetRtti().GetRttiName());
    if (factory == m_factories.end())
    {
        Platforms::Debug::Printf("Can't find dto factory of %s\n", dto.GetRtti().GetRttiName().c_str());
        return;
    }
    EventPublisher::Post(std::make_shared<FactoryGeometryCreated>(dto, factory->second(dto)));
}

void GeometryBuilder::OnDtoDeserialized(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<GenericDtoDeserialized, IEvent>(e);
    if (!ev) return;
    if (ev->GetRuid() != m_ruidDeserializing) return;
    if (ev->GetDtos().empty())
    {
        EventPublisher::Post(std::make_shared<BuildGeometryDataFailed>(m_policy.Name(), ErrorCode::deserializeFail));
        return;
    }
    CreateFromDto(ev->GetDtos()[0]);
}

void GeometryBuilder::OnDeserializeDtoFailed(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<DeserializeDtoFailed, IEvent>(e);
    if (!ev) return;
    if (ev->GetRuid() != m_ruidDeserializing) return;
    EventPublisher::Post(std::make_shared<BuildGeometryDataFailed>(m_policy.Name(), ev->GetErrorCode()));
}

void GeometryBuilder::OnFactoryGeometryCreated(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<FactoryGeometryCreated, IEvent>(e);
    if (!ev) return;
    if (ev->GetDto().GetId() != m_ruidInstancing) return;
    EventPublisher::Post(std::make_shared<GeometryDataBuilt>(m_policy.Name(), ev->GetGeometryData()));
}

void GeometryBuilder::DoRegisteringGeometryFactory(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<RegisterGeometryDtoFactory, ICommand>(c);
    if (!cmd) return;
    m_factories.insert_or_assign(cmd->GetRtti(), cmd->GetFactory());
}

void GeometryBuilder::DoUnRegisteringGeometryFactory(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<UnRegisterGeometryDtoFactory, ICommand>(c);
    if (!cmd) return;
    if (m_factories.find(cmd->GetRtti()) != m_factories.end()) m_factories.erase(cmd->GetRtti());
}
