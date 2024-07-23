#include "Portal.h"
#include "PortalDtos.h"
#include "PortalZoneNode.h"
#include "SceneGraphErrors.h"
#include "Culler.h"
#include "Camera.h"
#include "SceneGraphQueries.h"
#include "Platforms/PlatformLayerUtilities.h"
#include "PortalEvents.h"

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

Portal::Portal(const SpatialId& id) : Spatial(id)
{
    m_adjacentPortalZone = nullptr;
    m_factoryDesc = FactoryDesc(Portal::TYPE_RTTI.getName());
    m_isOpen = false;
    m_zoneLoadStatus = ZoneLoadStatus::None;
}

Portal::Portal(const SpatialId& id, const Engine::GenericDto& o) : Spatial(id, o)
{
    m_adjacentPortalZone = nullptr;
    PortalDto dto{ o };
    m_isOpen = dto.isOpen();
    m_adjacentZoneId = dto.adjacentZoneNodeId();
    m_zoneLoadStatus = ZoneLoadStatus::None;
}

Portal::~Portal()
{
}

std::shared_ptr<Portal> Portal::queryPortal(const SpatialId& id)
{
    assert(id.rtti().isDerived(Portal::TYPE_RTTI));
    return std::dynamic_pointer_cast<Portal, Spatial>(std::make_shared<QuerySpatial>(id)->dispatch());
}

std::shared_ptr<Portal> Portal::create(const SpatialId& id)
{
    return std::make_shared<Portal>(id);
}

std::shared_ptr<Portal> Portal::constitute(const SpatialId& id, const Engine::GenericDto& dto)
{
    return std::make_shared<Portal>(id, dto);
}

Enigma::Engine::GenericDto Portal::serializeDto()
{
    PortalDto dto(serializeSpatialDto());
    dto.isOpen(m_isOpen);
    dto.adjacentZoneNodeId(m_adjacentZoneId);
    return dto.toGenericDto();
}

void Portal::adjacentZone(const std::shared_ptr<PortalZoneNode>& zone)
{
    if (!zone) return;
    m_adjacentZoneId = zone->id();
    zone->setPortalParent(m_id);
    m_zoneLoadStatus = ZoneLoadStatus::Done;
    std::make_shared<PortalZoneAttached>(m_id, m_adjacentZoneId)->enqueue();
}

std::shared_ptr<PortalZoneNode> Portal::adjacentZone()
{
    if (m_adjacentZoneId.empty()) return nullptr;
    if (!m_adjacentPortalZone)
    {
        auto zone = std::dynamic_pointer_cast<PortalZoneNode>(Node::queryNode(m_adjacentZoneId));
        if (zone) adjacentZone(zone);
    }
    return m_adjacentPortalZone;
}

error Portal::onCullingVisible(Culler* culler, bool noCull)
{
    const auto zone = adjacentZone();
    if (!zone) return ErrorCode::ok;

    error er = zone->cullVisibleSet(culler, noCull);
    if (er) return er;

    return er;
}

error Portal::cullVisibleSet(Culler* culler, bool noCull)
{
    if (!canVisited()) return ErrorCode::dataNotReady;
    if (FATAL_LOG_EXPR((!culler) || (!culler->GetCamera()))) return ErrorCode::nullCullerCamera;

    if (!m_isOpen) return ErrorCode::ok;
    const auto zone = adjacentZone();
    if (!zone) return ErrorCode::ok;

    const bool isPortalPass = culler->IsVisible(&m_vecPortalQuadWorldPos[0], PORTAL_VERTEX_COUNT, true);
    if ((!noCull) && (!isPortalPass)) return ErrorCode::ok;  // not pass portal, so not in visible set

    if ((!noCull) && (m_quadWorldPlane.Normal().dot(culler->GetCamera()->eyeToLookatVector()) < 0))
        return ErrorCode::ok;  // not see through

    //Todo: 這裡要先把Frustum縮小再繼續

    const error er = onCullingVisible(culler, noCull);
    if (er) return er;

    return er;
}

error Portal::_updateWorldData(const MathLib::Matrix4& parentWorld)
{
    const error er = Spatial::_updateWorldData(parentWorld);
    if (er) return er;

    updatePortalQuad();

    return er;
}

SceneTraveler::TravelResult Portal::visitBy(SceneTraveler* traveler)
{
    if (!traveler) return SceneTraveler::TravelResult::InterruptError;

    SceneTraveler::TravelResult res = traveler->travelTo(thisSpatial());
    if (res != SceneTraveler::TravelResult::Continue) return res;  // don't go sub-tree

    if (const auto zone = adjacentZone())
    {
        res = zone->visitBy(traveler);
    }
    return res;
}

void Portal::updatePortalQuad()
{
    m_vecPortalQuadWorldPos[0] = m_mxWorldTransform.TransformCoord(s_vecPortalLocalQuad[0]);
    m_vecPortalQuadWorldPos[1] = m_mxWorldTransform.TransformCoord(s_vecPortalLocalQuad[1]);
    m_vecPortalQuadWorldPos[2] = m_mxWorldTransform.TransformCoord(s_vecPortalLocalQuad[2]);
    m_vecPortalQuadWorldPos[3] = m_mxWorldTransform.TransformCoord(s_vecPortalLocalQuad[3]);

    // 直接重算應該比TransformNormal快..
    m_quadWorldPlane = Plane3(m_vecPortalQuadWorldPos[0], m_vecPortalQuadWorldPos[1], m_vecPortalQuadWorldPos[2]);
}

