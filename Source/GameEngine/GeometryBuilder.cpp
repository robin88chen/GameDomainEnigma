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
#include <cassert>
#include <memory>

using namespace Enigma::Engine;
using namespace Enigma::Frameworks;

GeometryBuilder::GeometryBuilder(GeometryRepository* host)
{
    m_hostRepository = host;
    m_onDtoDeserialized = std::make_shared<EventSubscriber>([=](auto e) { this->OnDtoDeserialized(e); });
    EventPublisher::Subscribe(typeid(GenericDtoDeserialized), m_onDtoDeserialized);
    m_onDeserializeDtoFailed = std::make_shared<EventSubscriber>([=](auto e) { this->OnDeserializeDtoFailed(e); });
    EventPublisher::Subscribe(typeid(DeserializeDtoFailed), m_onDeserializeDtoFailed);
    m_onDtoGeometryCreated = std::make_shared<EventSubscriber>([=](auto e) { this->OnDtoGeometryCreated(e); });
    EventPublisher::Subscribe(typeid(FactoryGeometryCreated), m_onDtoGeometryCreated);
}

GeometryBuilder::~GeometryBuilder()
{
    EventPublisher::Unsubscribe(typeid(GenericDtoDeserialized), m_onDtoDeserialized);
    m_onDtoDeserialized = nullptr;
    EventPublisher::Unsubscribe(typeid(DeserializeDtoFailed), m_onDeserializeDtoFailed);
    m_onDeserializeDtoFailed = nullptr;
    EventPublisher::Unsubscribe(typeid(FactoryGeometryCreated), m_onDtoGeometryCreated);
    m_onDtoGeometryCreated = nullptr;
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
    else if (policy.GetDto())
    {
        CreateFromDto(policy.Name(), policy.GetDto().value());
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

void GeometryBuilder::CreateFromDto(const std::string& name, const GenericDto& dto)
{
    assert(m_hostRepository);
    m_ruidContracting = dto.GetId();
    CommandBus::Post(std::make_shared<InvokeDtoFactory>(dto));
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
    CreateFromDto(m_policy.Name(), ev->GetDtos()[0]);
}

void GeometryBuilder::OnDeserializeDtoFailed(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<DeserializeDtoFailed, IEvent>(e);
    if (!ev) return;
    if (ev->GetRuid() != m_ruidDeserializing) return;
    EventPublisher::Post(std::make_shared<BuildGeometryDataFailed>(m_policy.Name(), ev->GetErrorCode()));
}

void GeometryBuilder::OnDtoGeometryCreated(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<FactoryGeometryCreated, IEvent>(e);
    if (!ev) return;
    if (ev->GetDto().GetId() != m_ruidContracting) return;
    EventPublisher::Post(std::make_shared<GeometryDataBuilt>(m_policy.Name(), ev->GetGeometryData()));
}
