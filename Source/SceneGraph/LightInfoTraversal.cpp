#include "LightInfoTraversal.h"
#include "SpatialLightInfoQuery.h"
#include "Light.h"
#include "SceneGraphEvents.h"
#include "Frameworks/EventPublisher.h"

using namespace Enigma::SceneGraph;
using namespace Enigma::Frameworks;

DEFINE_RTTI(SceneGraph, LightInfoTraversal);

LightInfoTraversal::LightInfoTraversal(Frameworks::ServiceManager* srv_mngr) : ISystemService(srv_mngr)
{
    IMPLEMENT_RTTI(Enigma, SceneGraph, LightInfoTraversal, ISystemService);
    m_needTick = false;
    m_onLightInfoCreated = std::make_shared<EventSubscriber>([=](auto e) { this->OnLightInfoCreated(e); });
    EventPublisher::Subscribe(typeid(LightInfoCreated), m_onLightInfoCreated);
    m_onLightInfoDeleted = std::make_shared<EventSubscriber>([=](auto e) { this->OnLightInfoDeleted(e); });
    EventPublisher::Subscribe(typeid(LightInfoDeleted), m_onLightInfoDeleted);
}

LightInfoTraversal::~LightInfoTraversal()
{
    EventPublisher::Unsubscribe(typeid(LightInfoCreated), m_onLightInfoCreated);
    m_onLightInfoCreated = nullptr;
    EventPublisher::Unsubscribe(typeid(LightInfoDeleted), m_onLightInfoDeleted);
    m_onLightInfoDeleted = nullptr;
}

void LightInfoTraversal::QuerySpatialLightInfo(SpatialLightInfoQuery& query)
{
    if (m_lights.empty()) return;
    std::lock_guard locker{ m_mapLock };
    for (auto& kv : m_lights)
    {
        if (!kv.second.expired())
        {
            query.Test(kv.second.lock()->Info());
        }
    }
}

void LightInfoTraversal::OnLightInfoCreated(const IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<LightInfoCreated, IEvent>(e);
    if (!ev) return;
    if (!ev->GetLight()) return;
    std::lock_guard locker{ m_mapLock };
    m_lights.insert_or_assign(ev->GetLight()->GetSpatialName(), ev->GetLight());
}

void LightInfoTraversal::OnLightInfoDeleted(const IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<LightInfoDeleted, IEvent>(e);
    if (!ev) return;
    std::lock_guard locker{ m_mapLock };
    m_lights.erase(ev->GetLightName());
}
