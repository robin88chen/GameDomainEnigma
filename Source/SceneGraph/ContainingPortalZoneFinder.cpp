﻿#include "ContainingPortalZoneFinder.h"
#include "Spatial.h"
#include "PortalZoneNode.h"

using namespace Enigma::SceneGraph;

ContainingPortalZoneFinder::ContainingPortalZoneFinder(const MathLib::Vector3& targetWorldPos) : SceneTraveler()
{
    m_targetWorldPos = targetWorldPos;
    m_containingPortalZone = nullptr;
}

ContainingPortalZoneFinder::~ContainingPortalZoneFinder()
{
    m_containingPortalZone = nullptr;
}

SceneTraveler::TravelResult ContainingPortalZoneFinder::travelTo(const SpatialPtr& spatial)
{
    if ((!spatial) || (spatial->getWorldBound().isEmpty())) return SceneTraveler::TravelResult::InterruptError;
    if (spatial->getWorldBound().PointInside(m_targetWorldPos) == false) return SceneTraveler::TravelResult::TestFail;

    std::shared_ptr<PortalZoneNode> node = std::dynamic_pointer_cast<PortalZoneNode, Spatial>(spatial);
    if (node)
    {
        m_containingPortalZone = node;
    }

    return SceneTraveler::TravelResult::Continue;
}
