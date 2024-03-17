/*********************************************************************
 * \file   PortalManagementNode.h
 * \brief  Portal Mgt Node 加入在 Scene Root Node 下，Identity Matrix
 *          外部世界是一個Outside Zone, 加入在 Portal Mgt Node下，連結為 Outside Zone
 *          室內 Zone Node, 也都加入在 Portal Mgt Node下，並透過Portal 連結
 *          計算可見Node時，先找出攝影機所在的 Node (如果沒有室內Node，就取用室外Node),
 *          然後從起始 Node 開始 根據 Node Tree & Portal 的關係，找出 Visible Set
 *          也可以就把這Node 當作 Scene Root
 * \author Lancelot 'Robin' Chen
 * \date   March 2023
 *********************************************************************/
#ifndef _PORTAL_MANAGEMENTNODE_H
#define _PORTAL_MANAGEMENTNODE_H

#include "Node.h"
#include "Frameworks/CommandSubscriber.h"

namespace Enigma::SceneGraph
{
    class PortalZoneNode;

    class PortalManagementNode : public Node
    {
        DECLARE_EN_RTTI;
    public:
        PortalManagementNode(const SpatialId& id);
        PortalManagementNode(const SpatialId& id, const Engine::GenericDto& o);
        PortalManagementNode(const PortalManagementNode&) = delete;
        PortalManagementNode(PortalManagementNode&&) = delete;
        PortalManagementNode& operator=(const PortalManagementNode&) = delete;
        PortalManagementNode& operator=(PortalManagementNode&&) = delete;
        virtual ~PortalManagementNode() override;

        static std::shared_ptr<PortalManagementNode> create(const SpatialId& id);
        static std::shared_ptr<PortalManagementNode> constitute(const SpatialId& id, const Engine::GenericDto& dto);

        virtual Engine::GenericDto serializeDto() override;

        void attachOutsideZone(const std::shared_ptr<PortalZoneNode>& node);

        /** on cull visible, used by culler, for compute visible set, find start zone, then go to portal culling procedure  */
        virtual error onCullingVisible(Culler* culler, bool noCull) override;

    protected:
        void attachOutsideZone(const Frameworks::ICommandPtr& c);

    protected:
        SpatialId m_outsideZoneId;
        std::weak_ptr<PortalZoneNode> m_cachedStartZone;

        Frameworks::CommandSubscriberPtr m_attachOutsideZone;
    };
}

#endif // _PORTAL_MANAGEMENTNODE_H
