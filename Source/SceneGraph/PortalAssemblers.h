/*****************************************************************
 * \file   PortalAssemblers.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   June 2024
 ******************************************************************/
#ifndef PORTALASSEMBLERS_H
#define PORTALASSEMBLERS_H

#include "SceneGraphAssemblers.h"
#include "PortalDtos.h"

namespace Enigma::SceneGraph
{
    class Portal;
    class PortalZoneNode;

    class PortalAssembler
    {
    public:
        PortalAssembler(const SpatialId& id);

        const SpatialId& id() const { return m_id; }

        SpatialAssembler& spatial();

        PortalAssembler& factory(const Engine::FactoryDesc& factory);
        PortalAssembler& adjacentZoneNodeId(const SpatialId& adjacent_zone_node_id);
        PortalAssembler& isOpen(bool is_open);
        PortalAssembler& asNative(const std::string& file_at_path);

        Engine::GenericDto toGenericDto() const;
        PortalDto toPortalDto() const;
        std::shared_ptr<Portal> constitute();

    private:
        SpatialId m_id;
        SpatialAssembler m_spatialAssembler;
        PortalDto m_dto;
    };

    class PortalZoneNodeAssembler
    {
    public:
        PortalZoneNodeAssembler(const SpatialId& id);

        const SpatialId& id() const { return m_id; }

        LazyNodeAssembler& lazyNode();

        PortalZoneNodeAssembler& factory(const Engine::FactoryDesc& factory);
        PortalZoneNodeAssembler& portalParentId(const SpatialId& portal_parent_id);
        PortalZoneNodeAssembler& asDeferred(const std::string& file_name, const std::string& path_id);

        Engine::GenericDto toHydratedGenericDto() const;
        PortalZoneNodeDto toHydratedDto() const;
        std::shared_ptr<PortalZoneNode> constituteHydrated();
        Engine::GenericDto toDeHydratedGenericDto() const;
        PortalZoneNodeDto toDeHydratedDto() const;
        std::shared_ptr<PortalZoneNode> constituteDeHydrated();

    private:
        SpatialId m_id;
        LazyNodeAssembler m_lazyNodeAssembler;
        PortalZoneNodeDto m_dto;
    };

    class PortalManagementNodeAssembler
    {
    public:
        PortalManagementNodeAssembler(const SpatialId& id);

        const SpatialId& id() const { return m_id; }

        NodeAssembler& node();

        PortalManagementNodeAssembler& factory(const Engine::FactoryDesc& factory);
        PortalManagementNodeAssembler& outsideZoneNodeId(const SpatialId& outside_zone_node_id);
        PortalManagementNodeAssembler& asNative(const std::string& file_at_path);

        Engine::GenericDto toGenericDto() const;
        PortalManagementNodeDto toPortalManagementNodeDto() const;
        std::shared_ptr<PortalManagementNode> constitute();

    private:
        SpatialId m_id;
        NodeAssembler m_nodeAssembler;
        PortalManagementNodeDto m_dto;
    };
}

#endif // PORTALASSEMBLERS_H
