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
        PortalZoneNodeDto() = default;
        PortalZoneNodeDto(const LazyNodeDto& lazy_node_dto);

        static PortalZoneNodeDto FromGenericDto(const Engine::GenericDto& dto);
        Engine::GenericDto ToGenericDto() const;
    };

    class PortalDto : public SpatialDto
    {
    public:
        PortalDto() : m_isOpen(false) {};
        PortalDto(const SpatialDto& spatial_dto);

        [[nodiscard]] const std::string& AdjacentZoneNodeName() const { return m_adjacentZoneNodeName; }
        std::string& AdjacentZoneNodeName() { return m_adjacentZoneNodeName; }

        [[nodiscard]] bool IsOpen() const { return m_isOpen; }
        bool& IsOpen() { return m_isOpen; }

        static PortalDto FromGenericDto(const Engine::GenericDto& dto);
        Engine::GenericDto ToGenericDto();

    protected:
        std::string m_adjacentZoneNodeName;
        bool m_isOpen;
    };

    class PortalManagementNodeDto : public NodeDto
    {
    public:
        PortalManagementNodeDto() = default;
        PortalManagementNodeDto(const NodeDto& node_dto);

        static PortalManagementNodeDto FromGenericDto(const Engine::GenericDto& dto);
        Engine::GenericDto ToGenericDto();

        [[nodiscard]] const std::string& OutsideZoneNodeName() const { return m_outsideZoneNodeName; }
        std::string& OutsideZoneNodeName() { return m_outsideZoneNodeName; }

    protected:
        std::string m_outsideZoneNodeName;
    };
}

#endif // _PORTAL_DTOS_H
