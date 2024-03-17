/*********************************************************************
 * \file   PortalDtos.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   March 2023
 *********************************************************************/
#ifndef _PORTAL_DTOS_H
#define _PORTAL_DTOS_H

#include <string>
#include "SceneGraphDtos.h"

namespace Enigma::SceneGraph
{
    class PortalZoneNodeDto : public LazyNodeDto
    {
    public:
        PortalZoneNodeDto();
        PortalZoneNodeDto(const Engine::GenericDto& dto);
        PortalZoneNodeDto(const LazyNodeDto& lazy_node_dto);

        [[nodiscard]] const SpatialId& portalParentId() const { return m_portalParentId; }
        SpatialId& portalParentId() { return m_portalParentId; }

        Engine::GenericDto toGenericDto() const;

    protected:
        SpatialId m_portalParentId;
    };

    class PortalDto : public SpatialDto
    {
    public:
        PortalDto();
        PortalDto(const Engine::GenericDto& dto);
        PortalDto(const SpatialDto& spatial_dto);

        [[nodiscard]] const SpatialId& adjacentZoneNodeId() const { return m_adjacentZoneNodeId; }
        SpatialId& adjacentZoneNodeId() { return m_adjacentZoneNodeId; }

        [[nodiscard]] bool isOpen() const { return m_isOpen; }
        bool& isOpen() { return m_isOpen; }

        Engine::GenericDto toGenericDto();

    protected:
        SpatialId m_adjacentZoneNodeId;
        bool m_isOpen;
    };

    class PortalManagementNodeDto : public NodeDto
    {
    public:
        PortalManagementNodeDto();
        PortalManagementNodeDto(const Engine::GenericDto& dto);
        PortalManagementNodeDto(const NodeDto& node_dto);

        Engine::GenericDto toGenericDto();

        [[nodiscard]] const SpatialId& outsideZoneNodeId() const { return m_outsideZoneNodeId; }
        SpatialId& outsideZoneNodeId() { return m_outsideZoneNodeId; }

    protected:
        SpatialId m_outsideZoneNodeId;
    };
}

#endif // _PORTAL_DTOS_H
