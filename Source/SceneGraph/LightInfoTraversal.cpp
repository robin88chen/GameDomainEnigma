#include "LightInfoTraversal.h"
#include "SpatialLightInfoQuery.h"
#include "Light.h"
#include "LightEvents.h"
#include "SceneGraphQueries.h"
#include "SceneGraphErrors.h"
#include "Frameworks/EventPublisher.h"
#include "Frameworks/QueryDispatcher.h"

using namespace Enigma::SceneGraph;
using namespace Enigma::Frameworks;
using namespace Enigma::Engine;

DEFINE_RTTI(SceneGraph, LightInfoTraversal, ISystemService);

LightInfoTraversal::LightInfoTraversal(Frameworks::ServiceManager* srv_mngr) : ISystemService(srv_mngr)
{
    m_needTick = false;
    m_onLightInfoCreated = std::make_shared<EventSubscriber>([=](auto e) { this->onLightInfoCreated(e); });
    EventPublisher::subscribe(typeid(LightInfoCreated), m_onLightInfoCreated);
    m_onLightInfoDeleted = std::make_shared<EventSubscriber>([=](auto e) { this->onLightInfoDeleted(e); });
    EventPublisher::subscribe(typeid(LightInfoDeleted), m_onLightInfoDeleted);

    m_queryLightingStateAt = std::make_shared<QuerySubscriber>([=](const IQueryPtr& q) { this->queryLightingStateAt(q); });
    QueryDispatcher::subscribe(typeid(QueryLightingStateAt), m_queryLightingStateAt);
}

LightInfoTraversal::~LightInfoTraversal()
{
    EventPublisher::unsubscribe(typeid(LightInfoCreated), m_onLightInfoCreated);
    m_onLightInfoCreated = nullptr;
    EventPublisher::unsubscribe(typeid(LightInfoDeleted), m_onLightInfoDeleted);
    m_onLightInfoDeleted = nullptr;

    QueryDispatcher::unsubscribe(typeid(QueryLightingStateAt), m_queryLightingStateAt);
    m_queryLightingStateAt = nullptr;
}

SpatialRenderState LightInfoTraversal::queryLightingStateAt(const MathLib::Vector3& wolrd_position)
{
    RenderLightingState lighting_state;
    SpatialLightInfoQuery query;
    query.initSpatialPosition(wolrd_position);
    if (m_lights.empty()) return lighting_state;
    std::lock_guard locker{ m_mapLock };
    for (auto& kv : m_lights)
    {
        if (!kv.second.expired())
        {
            query.test(kv.second.lock()->info());
        }
    }
    if (query.getResultList().empty()) return lighting_state;

    std::vector<MathLib::Vector4> light_positions;
    std::vector<MathLib::ColorRGBA> light_colors;
    std::vector<MathLib::Vector4> light_attenuations;

    for (SpatialLightInfoQuery::LightInfoList::const_iterator iter = query.getResultList().begin();
        iter != query.getResultList().end(); ++iter)
    {
        switch ((*iter).m_lightInfo.lightType())
        {
        case LightInfo::LightType::Ambient:
            lighting_state.SetAmbientLightColor((*iter).m_lightInfo.getLightColor());
            break;
        case LightInfo::LightType::SunLight:
        case LightInfo::LightType::Directional:  //todo : if we want directional light, separate it from sun light
            lighting_state.SetSunLight((*iter).m_lightInfo.getLightDirection(), (*iter).m_lightInfo.getLightColor());
            break;
        case LightInfo::LightType::Point:
        {
            MathLib::Vector3 pos = (*iter).m_lightInfo.getLightPosition();
            light_positions.emplace_back(MathLib::Vector4(pos.x(), pos.y(), pos.z(), (*iter).m_lightInfo.getLightRange()));
            light_colors.emplace_back((*iter).m_lightInfo.getLightColor());
            MathLib::Vector3 attn = (*iter).m_lightInfo.getLightAttenuation();
            light_attenuations.emplace_back(MathLib::Vector4(attn.x(), attn.y(), attn.z(), 1.0f));
        }
        break;
        default:
            break;
        }
    }
    return lighting_state;
}

void LightInfoTraversal::onLightInfoCreated(const IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<LightInfoCreated, IEvent>(e);
    if (!ev) return;
    if (!ev->light()) return;
    std::lock_guard locker{ m_mapLock };
    m_lights.insert_or_assign(ev->light()->id(), ev->light());
}

void LightInfoTraversal::onLightInfoDeleted(const IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<LightInfoDeleted, IEvent>(e);
    if (!ev) return;
    std::lock_guard locker{ m_mapLock };
    m_lights.erase(ev->lightId());
}

void LightInfoTraversal::queryLightingStateAt(const Frameworks::IQueryPtr& q)
{
    if (!q) return;
    auto query = std::dynamic_pointer_cast<QueryLightingStateAt, IQuery>(q);
    if (!query) return;
    auto lighting_state = queryLightingStateAt(query->worldPosition());
    query->setResult(lighting_state);
}
