/*********************************************************************
 * \file   ContainingPortalZoneFinder.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   March 2023
 *********************************************************************/
#ifndef _CONTAINING_PORTAL_ZONE_FINDER_H
#define _CONTAINING_PORTAL_ZONE_FINDER_H

#include "SceneTraveler.h"
#include "MathLib/Vector3.h"

namespace Enigma::SceneGraph
{
    class PortalZoneNode;

    class ContainingPortalZoneFinder : public SceneTraveler
    {
    public:
        ContainingPortalZoneFinder(const MathLib::Vector3& targetWorldPos);
        ContainingPortalZoneFinder(const ContainingPortalZoneFinder&) = default;
        ContainingPortalZoneFinder(ContainingPortalZoneFinder&&) = default;
        ContainingPortalZoneFinder& operator=(const ContainingPortalZoneFinder&) = default;
        ContainingPortalZoneFinder& operator=(ContainingPortalZoneFinder&&) = default;
        virtual ~ContainingPortalZoneFinder() override;

        virtual TravelResult TravelTo(const SpatialPtr& spatial) override;

        std::shared_ptr<PortalZoneNode> GetContainingZone() { return m_containingPortalZone; };

    private:
        MathLib::Vector3 m_targetWorldPos;
        std::shared_ptr<PortalZoneNode> m_containingPortalZone;
    };
}

#endif // _CONTAINING_PORTAL_ZONE_FINDER_H
