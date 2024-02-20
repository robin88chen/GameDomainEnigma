#include "LightInfoTraversal.h"
#include "SpatialLightInfoQuery.h"
#include "Light.h"
#include "SceneGraphEvents.h"
#include "SpatialLightInfoRequest.h"
#include "SpatialLightInfoResponse.h"
#include "SceneGraphErrors.h"
#include "Frameworks/EventPublisher.h"
#include "Frameworks/ResponseBus.h"
#include "Frameworks/RequestBus.h"

using namespace Enigma::SceneGraph;
using namespace Enigma::Frameworks;
using namespace Enigma::Engine;

DEFINE_RTTI(SceneGraph, LightInfoTraversal, ISystemService);

LightInfoTraversal::LightInfoTraversal(Frameworks::ServiceManager* srv_mngr) : ISystemService(srv_mngr), m_isCurrentQuerying(false), m_requesterRuid()
{
    m_needTick = false;
    m_onLightInfoCreated = std::make_shared<EventSubscriber>([=](auto e) { this->OnLightInfoCreated(e); });
    EventPublisher::subscribe(typeid(LightInfoCreated), m_onLightInfoCreated);
    m_onLightInfoDeleted = std::make_shared<EventSubscriber>([=](auto e) { this->OnLightInfoDeleted(e); });
    EventPublisher::subscribe(typeid(LightInfoDeleted), m_onLightInfoDeleted);

    m_doQueryingLight = std::make_shared<RequestSubscriber>([=](auto r) { DoQueryingLight(r); });
    RequestBus::subscribe(typeid(RequestSpatialLightInfo), m_doQueryingLight);
}

LightInfoTraversal::~LightInfoTraversal()
{
    EventPublisher::unsubscribe(typeid(LightInfoCreated), m_onLightInfoCreated);
    m_onLightInfoCreated = nullptr;
    EventPublisher::unsubscribe(typeid(LightInfoDeleted), m_onLightInfoDeleted);
    m_onLightInfoDeleted = nullptr;

    RequestBus::unsubscribe(typeid(RequestSpatialLightInfo), m_doQueryingLight);
    m_doQueryingLight = nullptr;
}

ServiceResult LightInfoTraversal::onTick()
{
    if (m_isCurrentQuerying) return ServiceResult::Pendding;
    QueryNextRequest();
    return ServiceResult::Pendding;
}

void LightInfoTraversal::QueryNextRequest()
{
    if (m_isCurrentQuerying) return;

    {
        std::lock_guard locker{ m_queryRequestLock };
        if (m_queryRequests.empty())
        {
            m_needTick = false;
            return;
        }

        const auto request = m_queryRequests.front();
        m_queryRequests.pop_front();
        m_requesterRuid = request->getRuid();
        m_querySpatialPos = request->GetSpatialPos();
        m_isCurrentQuerying = true;
    }

    RenderLightingState lighting_state;
    SpatialLightInfoQuery query;
    query.InitSpatialPosition(m_querySpatialPos);
    QuerySpatialLightInfo(query);
    if (query.GetResultList().empty())
    {
        ResponseBus::post(std::make_shared<SpatialLightInfoResponse>(
            m_requesterRuid, lighting_state, ErrorCode::emptyLightQueryResult));
        return;
    }

    std::vector<MathLib::Vector4> light_positions;
    std::vector<MathLib::ColorRGBA> light_colors;
    std::vector<MathLib::Vector4> light_attenuations;

    for (SpatialLightInfoQuery::LightInfoList::const_iterator iter = query.GetResultList().begin();
        iter != query.GetResultList().end(); ++iter)
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
    lighting_state.SetPointLightArray(light_positions, light_colors, light_attenuations);
    ResponseBus::post(std::make_shared<SpatialLightInfoResponse>(
        m_requesterRuid, lighting_state, ErrorCode::ok));
    m_isCurrentQuerying = false;
}

void LightInfoTraversal::QuerySpatialLightInfo(SpatialLightInfoQuery& query)
{
    if (m_lights.empty()) return;
    std::lock_guard locker{ m_mapLock };
    for (auto& kv : m_lights)
    {
        if (!kv.second.expired())
        {
            query.test(kv.second.lock()->info());
        }
    }
}

void LightInfoTraversal::OnLightInfoCreated(const IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<LightInfoCreated, IEvent>(e);
    if (!ev) return;
    if (!ev->light()) return;
    std::lock_guard locker{ m_mapLock };
    m_lights.insert_or_assign(ev->light()->getSpatialName(), ev->light());
}

void LightInfoTraversal::OnLightInfoDeleted(const IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<LightInfoDeleted, IEvent>(e);
    if (!ev) return;
    std::lock_guard locker{ m_mapLock };
    m_lights.erase(ev->lightName());
}

void LightInfoTraversal::DoQueryingLight(const Frameworks::IRequestPtr& r)
{
    if (!r) return;
    auto req = std::dynamic_pointer_cast<RequestSpatialLightInfo, IRequest>(r);
    if (!req) return;
    std::lock_guard locker{ m_queryRequestLock };
    m_queryRequests.push_back(req);
    m_needTick = true;
}
