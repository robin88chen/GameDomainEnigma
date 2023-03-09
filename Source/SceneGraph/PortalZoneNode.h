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
#include <memory>

namespace Enigma::SceneGraph
{
    class PortalZoneNodeDto;

    class PortalZoneNode : public LazyNode
    {
        DECLARE_EN_RTTI;
    public:
        PortalZoneNode(const std::string& name);
        PortalZoneNode(const PortalZoneNodeDto& dto);
        PortalZoneNode(const PortalZoneNode&) = delete;
        PortalZoneNode(PortalZoneNode&&) = delete;
        PortalZoneNode& operator=(const PortalZoneNode&) = delete;
        PortalZoneNode& operator=(PortalZoneNode&&) = delete;
        virtual ~PortalZoneNode() override;

        virtual Engine::GenericDto SerializeDto() override;

        virtual error OnCullingVisible(Culler* culler, bool noCull) override;

    protected:
        // for zone graph traversal
        bool m_hasTraversed;
    };
    using PortalZoneNodePtr = std::shared_ptr<PortalZoneNode>;
}

#endif // _PORTAL_ZONE_NODE_H
