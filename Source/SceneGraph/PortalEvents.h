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
    class OutsideZoneAttached : public Frameworks::IEvent
    {
    public:
        OutsideZoneAttached(const SpatialId& managementNodeId, const SpatialId& zoneId)
            : m_managementNodeId(managementNodeId), m_zoneId(zoneId) {}

        const SpatialId& managementNodeId() const { return m_managementNodeId; }
        const SpatialId& zoneId() const { return m_zoneId; }

    protected:
        SpatialId m_managementNodeId;
        SpatialId m_zoneId;
    };
    class OutsideZoneAttachmentFailed : public Frameworks::IEvent
    {
    public:
        OutsideZoneAttachmentFailed(const SpatialId& managementNodeId, const SpatialId& zoneId, std::error_code error)
            : m_managementNodeId(managementNodeId), m_zoneId(zoneId), m_error(error) {}
        const SpatialId& managementNodeId() const { return m_managementNodeId; }
        const SpatialId& zoneId() const { return m_zoneId; }
        std::error_code error() const { return m_error; }

    protected:
        SpatialId m_managementNodeId;
        SpatialId m_zoneId;
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
