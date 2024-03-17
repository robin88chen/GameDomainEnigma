#include "Portal.h"
#include "PortalDtos.h"
#include "PortalZoneNode.h"
#include "SceneGraphErrors.h"
#include "Culler.h"
#include "Camera.h"
#include "SceneGraphQueries.h"
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

Portal::Portal(const SpatialId& id) : Spatial(id)
{
    m_factoryDesc = FactoryDesc(Portal::TYPE_RTTI.getName());
    m_isOpen = false;
    m_zoneLoadStatus = ZoneLoadStatus::None;
}

Portal::Portal(const SpatialId& id, const Engine::GenericDto& o) : Spatial(id, o)
{
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
    dto.isOpen() = m_isOpen;
    dto.adjacentZoneNodeId() = m_adjacentZoneId;
    return dto.toGenericDto();
}

void Portal::setAdjacentZone(const SpatialId& id)
{
    m_adjacentZoneId = id;
    if (auto zone = getAdjacentZone())
    {
        zone->setPortalParent(m_id);
    }
}

std::shared_ptr<PortalZoneNode> Portal::getAdjacentZone()
{
    if ((m_adjacentPortalZone.expired()) || (m_adjacentPortalZone.lock()->id() != m_adjacentZoneId))
    {
        m_zoneLoadStatus = ZoneLoadStatus::None;
        if (m_adjacentZoneId.isValid())
        {
            m_adjacentPortalZone = std::dynamic_pointer_cast<PortalZoneNode>(Node::queryNode(m_adjacentZoneId));
            if (!m_adjacentPortalZone.expired()) m_zoneLoadStatus = ZoneLoadStatus::Done;
        }
        else
        {
            m_adjacentPortalZone.reset();
        }
    }
    return m_adjacentPortalZone.lock();
}

error Portal::onCullingVisible(Culler* culler, bool noCull)
{
    const auto zone = getAdjacentZone();
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
    const auto zone = getAdjacentZone();
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

    if (const auto zone = getAdjacentZone())
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

