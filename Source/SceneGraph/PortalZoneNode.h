/*********************************************************************
 * \file   PortalZoneNode.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   March 2023
 *********************************************************************/
#ifndef _PORTAL_ZONE_NODE_H
#define _PORTAL_ZONE_NODE_H

#include "LazyNode.h"
#include "GameEngine/FactoryDesc.h"
#include <memory>

namespace Enigma::SceneGraph
{
    class SceneGraphRepository;
    class PortalZoneNode : public LazyNode
    {
        DECLARE_EN_RTTI;
    public:
        PortalZoneNode(const SpatialId& id);
        PortalZoneNode(const SpatialId& id, const Engine::GenericDto& o);
        //PortalZoneNode(const std::string& name, const Engine::FactoryDesc& factory_desc);
        //PortalZoneNode(const Engine::GenericDto& dto);
        PortalZoneNode(const PortalZoneNode&) = delete;
        PortalZoneNode(PortalZoneNode&&) = delete;
        PortalZoneNode& operator=(const PortalZoneNode&) = delete;
        PortalZoneNode& operator=(PortalZoneNode&&) = delete;
        virtual ~PortalZoneNode() override;

        static std::shared_ptr<PortalZoneNode> create(const SpatialId& id);
        static std::shared_ptr<PortalZoneNode> constitute(const SpatialId& id, const Engine::GenericDto& dto);

        virtual Engine::GenericDto serializeDto() override;
        virtual Engine::GenericDto serializeLaziedContent() override;
        virtual Engine::GenericDto serializeAsLaziness() override;

        virtual error onCullingVisible(Culler* culler, bool noCull) override;

        virtual void setPortalParent(const SpatialId& id);

    protected:
        SpatialId m_portalParentId;
        //std::weak_ptr<Spatial> m_portalParent; // either portal or portal management node
        // for zone graph traversal
        bool m_hasTraversed;
    };
    using PortalZoneNodePtr = std::shared_ptr<PortalZoneNode>;
}

#endif // _PORTAL_ZONE_NODE_H
