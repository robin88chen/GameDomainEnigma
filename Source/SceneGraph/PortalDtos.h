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
    /*class PortalZoneNodeDto : public LazyNodeDto
    {
    public:
        PortalZoneNodeDto();
        PortalZoneNodeDto(const Engine::GenericDto& dto);
        PortalZoneNodeDto(const LazyNodeDto& lazy_node_dto);

        [[nodiscard]] const SpatialId& portalParentId() const { return m_portalParentId; }
        void portalParentId(const SpatialId& id) { m_portalParentId = id; }

        Engine::GenericDto toGenericDto() const;

    protected:
        SpatialId m_portalParentId;
    };*/

    /*class OutRegionNodeDto : public LazyNodeDto
    {
    public:
        OutRegionNodeDto();
        OutRegionNodeDto(const Engine::GenericDto& dto);
        OutRegionNodeDto(const LazyNodeDto& lazy_node_dto);

        [[nodiscard]] const SpatialId& ownerManagementId() const { return m_ownerManagementId; }
        void ownerManagementId(const SpatialId& id) { m_ownerManagementId = id; }

        Engine::GenericDto toGenericDto() const;

    protected:
        SpatialId m_ownerManagementId;
    };*/

    /*class PortalDto : public SpatialDto
    {
    public:
        PortalDto();
        PortalDto(const Engine::GenericDto& dto);
        PortalDto(const SpatialDto& spatial_dto);

        [[nodiscard]] const SpatialId& adjacentZoneNodeId() const { return m_adjacentZoneNodeId; }
        void adjacentZoneNodeId(const SpatialId& id) { m_adjacentZoneNodeId = id; }

        [[nodiscard]] bool isOpen() const { return m_isOpen; }
        void isOpen(bool open) { m_isOpen = open; }

        Engine::GenericDto toGenericDto();

    protected:
        SpatialId m_adjacentZoneNodeId;
        bool m_isOpen;
    };*/

    /*class PortalManagementNodeDto : public NodeDto
    {
    public:
        PortalManagementNodeDto();
        PortalManagementNodeDto(const Engine::GenericDto& dto);
        PortalManagementNodeDto(const NodeDto& node_dto);

        Engine::GenericDto toGenericDto();

        [[nodiscard]] const std::optional<SpatialId>& outsideRegionId() const { return m_outsideRegionId; }
        void outsideRegionId(const SpatialId& outside_id) { m_outsideRegionId = outside_id; }

    protected:
        std::optional<SpatialId> m_outsideRegionId;
    };*/
}

#endif // _PORTAL_DTOS_H
