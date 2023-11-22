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
    class PortalZoneNode : public LazyNode
    {
        DECLARE_EN_RTTI;
    public:
        PortalZoneNode(const std::string& name, const Engine::FactoryDesc& factory_desc);
        PortalZoneNode(const Engine::GenericDto& dto);
        PortalZoneNode(const PortalZoneNode&) = delete;
        PortalZoneNode(PortalZoneNode&&) = delete;
        PortalZoneNode& operator=(const PortalZoneNode&) = delete;
        PortalZoneNode& operator=(PortalZoneNode&&) = delete;
        virtual ~PortalZoneNode() override;

        virtual Engine::GenericDto serializeDto() override;

        virtual error onCullingVisible(Culler* culler, bool noCull) override;

    protected:
        // for zone graph traversal
        bool m_hasTraversed;
    };
    using PortalZoneNodePtr = std::shared_ptr<PortalZoneNode>;
}

#endif // _PORTAL_ZONE_NODE_H
