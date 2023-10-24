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
    EventPublisher::Subscribe(typeid(LightInfoCreated), m_onLightInfoCreated);
    m_onLightInfoDeleted = std::make_shared<EventSubscriber>([=](auto e) { this->OnLightInfoDeleted(e); });
    EventPublisher::Subscribe(typeid(LightInfoDeleted), m_onLightInfoDeleted);

    m_doQueryingLight = std::make_shared<RequestSubscriber>([=](auto r) { DoQueryingLight(r); });
    RequestBus::Subscribe(typeid(RequestSpatialLightInfo), m_doQueryingLight);
}

LightInfoTraversal::~LightInfoTraversal()
{
    EventPublisher::Unsubscribe(typeid(LightInfoCreated), m_onLightInfoCreated);
    m_onLightInfoCreated = nullptr;
    EventPublisher::Unsubscribe(typeid(LightInfoDeleted), m_onLightInfoDeleted);
    m_onLightInfoDeleted = nullptr;

    RequestBus::Unsubscribe(typeid(RequestSpatialLightInfo), m_doQueryingLight);
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
        m_requesterRuid = request->GetRuid();
        m_querySpatialPos = request->GetSpatialPos();
        m_isCurrentQuerying = true;
    }

    RenderLightingState lighting_state;
    SpatialLightInfoQuery query;
    query.InitSpatialPosition(m_querySpatialPos);
    QuerySpatialLightInfo(query);
    if (query.GetResultList().empty())
    {
        ResponseBus::Post(std::make_shared<SpatialLightInfoResponse>(
            m_requesterRuid, lighting_state, ErrorCode::emptyLightQueryResult));
        return;
    }

    std::vector<MathLib::Vector4> light_positions;
    std::vector<MathLib::ColorRGBA> light_colors;
    std::vector<MathLib::Vector4> light_attenuations;

    for (SpatialLightInfoQuery::LightInfoList::const_iterator iter = query.GetResultList().begin();
        iter != query.GetResultList().end(); ++iter)
    {
        switch ((*iter).m_lightInfo.GetLightType())
        {
        case LightInfo::LightType::Ambient:
            lighting_state.SetAmbientLightColor((*iter).m_lightInfo.GetLightColor());
            break;
        case LightInfo::LightType::SunLight:
        case LightInfo::LightType::Directional:  //todo : if we want directional light, separate it from sun light
            lighting_state.SetSunLight((*iter).m_lightInfo.GetLightDirection(), (*iter).m_lightInfo.GetLightColor());
            break;
        case LightInfo::LightType::Point:
        {
            MathLib::Vector3 pos = (*iter).m_lightInfo.GetLightPosition();
            light_positions.emplace_back(MathLib::Vector4(pos.X(), pos.Y(), pos.Z(), (*iter).m_lightInfo.GetLightRange()));
            light_colors.emplace_back((*iter).m_lightInfo.GetLightColor());
            MathLib::Vector3 attn = (*iter).m_lightInfo.GetLightAttenuation();
            light_attenuations.emplace_back(MathLib::Vector4(attn.X(), attn.Y(), attn.Z(), 1.0f));
        }
        break;
        default:
            break;
        }
    }
    lighting_state.SetPointLightArray(light_positions, light_colors, light_attenuations);
    ResponseBus::Post(std::make_shared<SpatialLightInfoResponse>(
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

void LightInfoTraversal::DoQueryingLight(const Frameworks::IRequestPtr& r)
{
    if (!r) return;
    auto req = std::dynamic_pointer_cast<RequestSpatialLightInfo, IRequest>(r);
    if (!req) return;
    std::lock_guard locker{ m_queryRequestLock };
    m_queryRequests.push_back(req);
    m_needTick = true;
}
