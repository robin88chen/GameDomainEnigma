#include "SpatialRenderState.h"
#include "SpatialLightInfoQuery.h"
#include "SpatialLightInfoRequest.h"
#include "SpatialLightInfoResponse.h"
#include "Frameworks/RequestBus.h"
#include "Frameworks/ResponseBus.h"
#include "GameEngine/RenderLightingState.h"

using namespace Enigma::SceneGraph;
using namespace Enigma::Frameworks;

SpatialRenderState::SpatialRenderState() : m_requestRuid()
{
    m_onLightingStateResponse = nullptr;
}

SpatialRenderState::SpatialRenderState(const Engine::RenderLightingState& lightingState) : m_requestRuid()
{
    m_lightingState = lightingState;
    m_onLightingStateResponse = nullptr;
}

SpatialRenderState::~SpatialRenderState()
{
    if (m_onLightingStateResponse)
    {
        ResponseBus::unsubscribe(typeid(SpatialLightInfoResponse), m_onLightingStateResponse);
        m_onLightingStateResponse = nullptr;
    }
}

void SpatialRenderState::QueryLightingState(const MathLib::Vector3& spatialPos)
{
    auto request = std::make_shared<RequestSpatialLightInfo>(spatialPos);
    m_requestRuid = request->getRuid();

    m_onLightingStateResponse = std::make_shared<ResponseSubscriber>([=](auto r) { OnLightingStateResponse(r); });
    ResponseBus::subscribe(typeid(SpatialLightInfoResponse), m_onLightingStateResponse);

    RequestBus::post(request);
}

void SpatialRenderState::OnLightingStateResponse(const Frameworks::IResponsePtr& r)
{
    if (!r) return;
    auto res = std::dynamic_pointer_cast<SpatialLightInfoResponse, IResponse>(r);
    if (!res) return;
    if (res->getRequestRuid() != m_requestRuid) return;

    if (!res->GetErrorCode()) m_lightingState = res->GetLightingState();
}
