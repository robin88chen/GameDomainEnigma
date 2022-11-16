#include "GeometryBuilder.h"
#include "GeometryDataEvents.h"
#include "GeometryRepository.h"
#include "GeometryDataPolicy.h"
#include "Frameworks/EventPublisher.h"
#include "Frameworks/EventSubscriber.h"
#include "ContractDeserializer.h"
#include "ContractEvents.h"
#include <cassert>
#include <memory>

using namespace Enigma::Engine;
using namespace Enigma::Frameworks;

GeometryBuilder::GeometryBuilder(GeometryRepository* host)
{
    m_hostRepository = host;
    m_onContractDeserialized = std::make_shared<EventSubscriber>([=](auto e) { this->OnContractDeserialized(e); });
    EventPublisher::Subscribe(typeid(ContractDeserialized), m_onContractDeserialized);
}

GeometryBuilder::~GeometryBuilder()
{
    EventPublisher::Unsubscribe(typeid(ContractDeserialized), m_onContractDeserialized);
    m_onContractDeserialized = nullptr;
}

void GeometryBuilder::BuildGeometry(const GeometryDataPolicy& policy)
{
    assert(m_hostRepository);
    if (m_hostRepository->HasGeometryData(policy.Name()))
    {
        EventPublisher::Post(std::make_shared<GeometryDataCreated>(policy.Name(),
            m_hostRepository->QueryGeometryData(policy.Name())));
    }
    else if (policy.GetContract())
    {
        CreateFromContract(policy.Name(), policy.GetContract().value());
    }
    else
    {
        m_policy = policy;
        m_ruidDeserializing = Ruid::Generate();
        policy.GetDeserializer().InvokeDeserialize(m_ruidDeserializing);
    }
}

void GeometryBuilder::CreateFromContract(const std::string& name, const Contract& contract)
{
    assert(m_hostRepository);
    EventPublisher::Post(std::make_shared<GeometryDataCreated>(name, m_hostRepository->Create(contract)));
}

void GeometryBuilder::OnContractDeserialized(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<ContractDeserialized, IEvent>(e);
    if (!ev) return;
    if (ev->GetRuid() != m_ruidDeserializing) return;
    CreateFromContract(m_policy.Name(), ev->GetContract());
}

