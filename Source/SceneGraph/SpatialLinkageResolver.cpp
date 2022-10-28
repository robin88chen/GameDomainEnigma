#include "SpatialLinkageResolver.h"
#include "Frameworks/EventPublisher.h"
#include "SceneGraphEvents.h"
#include <memory>
using namespace Enigma::SceneGraph;
using namespace Enigma::Engine;
using namespace Enigma::Frameworks;

SpatialLinkageResolver::SpatialLinkageResolver() : ContractedLinkageResolver<Spatial>()
{
    m_onContractedSpatialCreated = nullptr;
}

SpatialLinkageResolver::SpatialLinkageResolver(const Engine::FactoryQuery<Spatial>& query) : ContractedLinkageResolver<Spatial>(query)
{
    m_onContractedSpatialCreated =
        std::make_shared<EventSubscriber>([=](auto c) { this->OnContractedSpatialCreated(c); });
    EventPublisher::Subscribe(typeid(ContractedSpatialCreated), m_onContractedSpatialCreated);
}

SpatialLinkageResolver::SpatialLinkageResolver(const SpatialLinkageResolver& resolver)
    : ContractedLinkageResolver<Spatial>(dynamic_cast<const ContractedLinkageResolver<Spatial>&>(resolver))
{
    m_onContractedSpatialCreated =
        std::make_shared<EventSubscriber>([=](auto c) { this->OnContractedSpatialCreated(c); });
    EventPublisher::Subscribe(typeid(ContractedSpatialCreated), m_onContractedSpatialCreated);
}

SpatialLinkageResolver::~SpatialLinkageResolver()
{
    if (m_onContractedSpatialCreated)
    {
        EventPublisher::Unsubscribe(typeid(ContractedSpatialCreated), m_onContractedSpatialCreated);
        m_onContractedSpatialCreated = nullptr;
    }
}

SpatialLinkageResolver& SpatialLinkageResolver::operator=(const SpatialLinkageResolver& resolver)
{
    m_query = resolver.m_query;
    m_linkageResolverTable = resolver.m_linkageResolverTable;
    m_onContractedSpatialCreated =
        std::make_shared<EventSubscriber>([=](auto c) { this->OnContractedSpatialCreated(c); });
    EventPublisher::Subscribe(typeid(ContractedSpatialCreated), m_onContractedSpatialCreated);
    return *this;
}

void SpatialLinkageResolver::OnContractedSpatialCreated(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<ContractedSpatialCreated, IEvent>(e);
    if (!ev) return;
    InvokeLinkageResolver(ev->GetName(), ev->GetSpatial());
}
