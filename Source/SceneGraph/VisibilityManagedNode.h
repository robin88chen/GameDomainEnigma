/*********************************************************************
 * \file   VisibilityManagedNode.h
 * \brief  開闊世界地形上，以可見範圍控制的 Lazy Node,
 *         類似 Portal Zone Node, 但增加了釋放機制
 *         會用在廣大的世界地圖上
 * \author Lancelot 'Robin' Chen
 * \date   March 2023
 *********************************************************************/
#ifndef _VISIBILITY_MANAGED_NODE_H
#define _VISIBILITY_MANAGED_NODE_H

#include "LazyNode.h"

namespace Enigma::SceneGraph
{
    class VisibilityManagedNode : public LazyNode
    {
        DECLARE_EN_RTTI;
    public:
        VisibilityManagedNode(const std::string& name);
        VisibilityManagedNode(const Engine::GenericDto& dto);
        VisibilityManagedNode(const VisibilityManagedNode&) = delete;
        VisibilityManagedNode(VisibilityManagedNode&&) = delete;
        virtual ~VisibilityManagedNode() override;
        VisibilityManagedNode& operator=(const VisibilityManagedNode&) = delete;
        VisibilityManagedNode& operator=(VisibilityManagedNode&&) = delete;

        virtual Engine::GenericDto SerializeDto() override;

        virtual error OnCullingVisible(Culler* culler, bool noCull) override;
        virtual void OnCullingCompleteNotVisible(Culler* culler) override;

        void ReleaseDeferredContent();
    };
}

#endif // _VISIBILITY_MANAGED_NODE_H
