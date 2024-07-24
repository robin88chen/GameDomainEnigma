/*****************************************************************
 * \file   PortalEvents.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   July 2024
 ******************************************************************/
#ifndef PORTAL_EVENTS_H
#define PORTAL_EVENTS_H

#include "Frameworks/Event.h"
#include "SpatialId.h"
#include <system_error>

namespace Enigma::SceneGraph
{
    class OutsideRegionAttached : public Frameworks::IEvent
    {
    public:
        OutsideRegionAttached(const SpatialId& managementNodeId, const SpatialId& regionId)
            : m_managementNodeId(managementNodeId), m_regionId(regionId) {}

        const SpatialId& managementNodeId() const { return m_managementNodeId; }
        const SpatialId& regionId() const { return m_regionId; }

    protected:
        SpatialId m_managementNodeId;
        SpatialId m_regionId;
    };
    class OutsideRegionAttachmentFailed : public Frameworks::IEvent
    {
    public:
        OutsideRegionAttachmentFailed(const SpatialId& managementNodeId, const SpatialId& regionId, std::error_code error)
            : m_managementNodeId(managementNodeId), m_regionId(regionId), m_error(error) {}
        const SpatialId& managementNodeId() const { return m_managementNodeId; }
        const SpatialId& regionId() const { return m_regionId; }
        std::error_code error() const { return m_error; }

    protected:
        SpatialId m_managementNodeId;
        SpatialId m_regionId;
        std::error_code m_error;
    };
    class PortalZoneAttached : public Frameworks::IEvent
    {
    public:
        PortalZoneAttached(const SpatialId& portalParentId, const SpatialId& zoneId)
            : m_portalParentId(portalParentId), m_zoneId(zoneId) {}
        const SpatialId& portalParentId() const { return m_portalParentId; }
        const SpatialId& zoneId() const { return m_zoneId; }

    protected:
        SpatialId m_portalParentId;
        SpatialId m_zoneId;
    };
    class PortalZoneAttachmentFailed : public Frameworks::IEvent
    {
    public:
        PortalZoneAttachmentFailed(const SpatialId& portalParentId, const SpatialId& zoneId, std::error_code error)
            : m_portalParentId(portalParentId), m_zoneId(zoneId), m_error(error) {}
        const SpatialId& portalParentId() const { return m_portalParentId; }
        const SpatialId& zoneId() const { return m_zoneId; }
        std::error_code error() const { return m_error; }

    protected:
        SpatialId m_portalParentId;
        SpatialId m_zoneId;
        std::error_code m_error;
    };
}

#endif // PORTAL_EVENTS_H
