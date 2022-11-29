#include "SpatialLinkageResolver.h"
#include "Frameworks/EventPublisher.h"
#include "SceneGraphEvents.h"
#include <memory>
using namespace Enigma::SceneGraph;
using namespace Enigma::Engine;
using namespace Enigma::Frameworks;

SpatialLinkageResolver::SpatialLinkageResolver() : FactoryLinkageResolver<Spatial>()
{
    m_onFactorySpatialCreated = nullptr;
}

SpatialLinkageResolver::SpatialLinkageResolver(const Engine::FactoryQuery<Spatial>& query) : FactoryLinkageResolver<Spatial>(query)
{
    m_onFactorySpatialCreated =
        std::make_shared<EventSubscriber>([=](auto c) { this->OnFactorySpatialCreated(c); });
    EventPublisher::Subscribe(typeid(FactorySpatialCreated), m_onFactorySpatialCreated);
}

SpatialLinkageResolver::SpatialLinkageResolver(const SpatialLinkageResolver& resolver)
    : FactoryLinkageResolver<Spatial>(dynamic_cast<const FactoryLinkageResolver<Spatial>&>(resolver))
{
    m_onFactorySpatialCreated =
        std::make_shared<EventSubscriber>([=](auto c) { this->OnFactorySpatialCreated(c); });
    EventPublisher::Subscribe(typeid(FactorySpatialCreated), m_onFactorySpatialCreated);
}

SpatialLinkageResolver::~SpatialLinkageResolver()
{
    if (m_onFactorySpatialCreated)
    {
        EventPublisher::Unsubscribe(typeid(FactorySpatialCreated), m_onFactorySpatialCreated);
        m_onFactorySpatialCreated = nullptr;
    }
}

SpatialLinkageResolver& SpatialLinkageResolver::operator=(const SpatialLinkageResolver& resolver)
{
    m_query = resolver.m_query;
    m_linkageResolverTable = resolver.m_linkageResolverTable;
    m_onFactorySpatialCreated =
        std::make_shared<EventSubscriber>([=](auto c) { this->OnFactorySpatialCreated(c); });
    EventPublisher::Subscribe(typeid(FactorySpatialCreated), m_onFactorySpatialCreated);
    return *this;
}

void SpatialLinkageResolver::ClearResolvers()
{
    m_linkageResolverTable.clear();
}

void SpatialLinkageResolver::OnFactorySpatialCreated(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<FactorySpatialCreated, IEvent>(e);
    if (!ev) return;
    InvokeLinkageResolver(ev->GetSpatial()->GetSpatialName(), ev->GetSpatial());
}
