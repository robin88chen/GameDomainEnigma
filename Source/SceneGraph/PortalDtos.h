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
        PortalZoneNodeDto(const LazyNodeDto& lazy_node_dto);

        static PortalZoneNodeDto fromGenericDto(const Engine::GenericDto& dto);
        Engine::GenericDto toGenericDto() const;
    };

    class PortalDto : public SpatialDto
    {
    public:
        PortalDto();
        PortalDto(const SpatialDto& spatial_dto);

        [[nodiscard]] const std::string& adjacentZoneNodeName() const { return m_adjacentZoneNodeName; }
        std::string& adjacentZoneNodeName() { return m_adjacentZoneNodeName; }

        [[nodiscard]] bool isOpen() const { return m_isOpen; }
        bool& isOpen() { return m_isOpen; }

        static PortalDto fromGenericDto(const Engine::GenericDto& dto);
        Engine::GenericDto toGenericDto();

    protected:
        std::string m_adjacentZoneNodeName;
        bool m_isOpen;
    };

    class PortalManagementNodeDto : public NodeDto
    {
    public:
        PortalManagementNodeDto();
        PortalManagementNodeDto(const NodeDto& node_dto);

        static PortalManagementNodeDto fromGenericDto(const Engine::GenericDto& dto);
        Engine::GenericDto toGenericDto();

        [[nodiscard]] const std::string& outsideZoneNodeName() const { return m_outsideZoneNodeName; }
        std::string& outsideZoneNodeName() { return m_outsideZoneNodeName; }

    protected:
        std::string m_outsideZoneNodeName;
    };
}

#endif // _PORTAL_DTOS_H
