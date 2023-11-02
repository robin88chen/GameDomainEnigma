#include "Portal.h"
#include "PortalDtos.h"
#include "PortalZoneNode.h"
#include "SceneGraphErrors.h"
#include "Culler.h"
#include "Camera.h"
#include "Platforms/PlatformLayerUtilities.h"

using namespace Enigma::SceneGraph;
using namespace Enigma::MathLib;
using namespace Enigma::Engine;

// 預設的通道，可見的位置是在通道的+z方向
Vector3 s_vecPortalLocalQuad[PORTAL_VERTEX_COUNT] =
{
    Vector3(-0.5f, -0.5f, 0.0f), Vector3(0.5f, -0.5f, 0.0f),
    Vector3(0.5f,  0.5f, 0.0f), Vector3(-0.5f, 0.5f, 0.0f)
};

DEFINE_RTTI(SceneGraph, Portal, Spatial);

Portal::Portal(const std::string& name) : Spatial(name)
{
    m_factoryDesc = FactoryDesc(Portal::TYPE_RTTI.getName());
    m_isOpen = false;
    m_zoneLoadStatus = ZoneLoadStatus::None;
}

Portal::Portal(const GenericDto& o) : Spatial(o)
{
    PortalDto dto = PortalDto::FromGenericDto(o);
    m_isOpen = dto.IsOpen();
    m_zoneLoadStatus = ZoneLoadStatus::None;
}

Portal::~Portal()
{
}

Enigma::Engine::GenericDto Portal::serializeDto()
{
    PortalDto dto(serializeSpatialDto());
    dto.IsOpen() = m_isOpen;
    if (!m_adjacentPortalZone.expired()) dto.AdjacentZoneNodeName() = m_adjacentPortalZone.lock()->getSpatialName();
    return dto.ToGenericDto();
}

void Portal::resolveFactoryLinkage(const Engine::GenericDto& dto, Engine::FactoryLinkageResolver<Spatial>& resolver)
{
    PortalDto portalDto = PortalDto::FromGenericDto(dto);
    resolver.TryResolveLinkage(portalDto.AdjacentZoneNodeName(), [lifetime = weak_from_this()](auto sp)
        {
            if (!lifetime.expired())
                std::dynamic_pointer_cast<Portal, Spatial>(lifetime.lock())->SetAdjacentZone(std::dynamic_pointer_cast<PortalZoneNode, Spatial>(sp));
        });
}

void Portal::SetAdjacentZone(const std::shared_ptr<PortalZoneNode>& node)
{
    m_adjacentPortalZone = node;
    if (!m_adjacentPortalZone.expired())
    {
        m_zoneLoadStatus = ZoneLoadStatus::Done;
    }
}

error Portal::onCullingVisible(Culler* culler, bool noCull)
{
    if (m_adjacentPortalZone.expired()) return ErrorCode::ok;

    error er = m_adjacentPortalZone.lock()->cullVisibleSet(culler, noCull);
    if (er) return er;

    return er;
}

error Portal::cullVisibleSet(Culler* culler, bool noCull)
{
    if (!canVisited()) return ErrorCode::dataNotReady;
    if (FATAL_LOG_EXPR((!culler) || (!culler->GetCamera()))) return ErrorCode::nullCullerCamera;

    if (!m_isOpen) return ErrorCode::ok;
    if (m_adjacentPortalZone.expired()) return ErrorCode::ok;

    bool isPortalPass = culler->IsVisible(&m_vecPortalQuadWorldPos[0], PORTAL_VERTEX_COUNT, true);
    if ((!noCull) && (!isPortalPass)) return ErrorCode::ok;  // not pass portal, so not in visible set

    if ((!noCull) && (m_quadWorldPlane.Normal().Dot(culler->GetCamera()->GetEyeToLookatVector()) < 0))
        return ErrorCode::ok;  // not see through

    //Todo: 這裡要先把Frustum縮小再繼續

    error er = onCullingVisible(culler, noCull);
    if (er) return er;

    return er;
}

error Portal::_updateWorldData(const MathLib::Matrix4& parentWorld)
{
    error er = Spatial::_updateWorldData(parentWorld);
    if (er) return er;

    UpdatePortalQuad();

    return er;
}

SceneTraveler::TravelResult Portal::visitBy(SceneTraveler* traveler)
{
    if (!traveler) return SceneTraveler::TravelResult::InterruptError;

    SceneTraveler::TravelResult res = traveler->TravelTo(thisSpatial());
    if (res != SceneTraveler::TravelResult::Continue) return res;  // don't go sub-tree

    if (!m_adjacentPortalZone.expired())
    {
        res = m_adjacentPortalZone.lock()->visitBy(traveler);
    }
    return res;
}

void Portal::UpdatePortalQuad()
{
    m_vecPortalQuadWorldPos[0] = m_mxWorldTransform.TransformCoord(s_vecPortalLocalQuad[0]);
    m_vecPortalQuadWorldPos[1] = m_mxWorldTransform.TransformCoord(s_vecPortalLocalQuad[1]);
    m_vecPortalQuadWorldPos[2] = m_mxWorldTransform.TransformCoord(s_vecPortalLocalQuad[2]);
    m_vecPortalQuadWorldPos[3] = m_mxWorldTransform.TransformCoord(s_vecPortalLocalQuad[3]);

    // 直接重算應該比TransformNormal快..
    m_quadWorldPlane = Plane3(m_vecPortalQuadWorldPos[0], m_vecPortalQuadWorldPos[1], m_vecPortalQuadWorldPos[2]);
}

