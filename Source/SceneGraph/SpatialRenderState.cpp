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
}

SpatialRenderState::~SpatialRenderState()
{
}

void SpatialRenderState::QueryLightingState(const MathLib::Vector3& spatialPos)
{
    auto request = std::make_shared<RequestSpatialLightInfo>(spatialPos);
    m_requestRuid = request->GetRuid();

    m_onLightingStateResponse = std::make_shared<ResponseSubscriber>([=](auto r) { OnLightingStateResponse(r); });
    ResponseBus::Subscribe(typeid(SpatialLightInfoResponse), m_onLightingStateResponse);

    RequestBus::Post(request);
}

void SpatialRenderState::OnLightingStateResponse(const Frameworks::IResponsePtr& r)
{
    if (!r) return;
    auto res = std::dynamic_pointer_cast<SpatialLightInfoResponse, IResponse>(r);
    if (!res) return;
    if (res->GetRequestRuid() != m_requestRuid) return;
    ResponseBus::Unsubscribe(typeid(SpatialLightInfoResponse), m_onLightingStateResponse);
    m_onLightingStateResponse = nullptr;

    if (!res->GetErrorCode()) m_lightingState = res->GetLightingState();
}
