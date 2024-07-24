/*********************************************************************
 * \file   OutRegionNode.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   July 2024
 *********************************************************************/
#ifndef OUT_REGION_NODE_H
#define OUT_REGION_NODE_H

#include "LazyNode.h"

namespace Enigma::SceneGraph
{
    class OutRegionNode : public LazyNode
    {
        DECLARE_EN_RTTI;
    public:
        OutRegionNode(const SpatialId& id);
        OutRegionNode(const SpatialId& id, const Engine::GenericDto& o);
        OutRegionNode(const OutRegionNode&) = delete;
        OutRegionNode(OutRegionNode&&) = delete;
        OutRegionNode& operator=(const OutRegionNode&) = delete;
        OutRegionNode& operator=(OutRegionNode&&) = delete;
        virtual ~OutRegionNode() override;

        static std::shared_ptr<OutRegionNode> create(const SpatialId& id);
        static std::shared_ptr<OutRegionNode> constitute(const SpatialId& id, const Engine::GenericDto& dto);

        virtual Engine::GenericDto serializeDto() override;
        virtual Engine::GenericDto serializeLaziedContent() override;
        virtual Engine::GenericDto serializeAsLaziness() override;

        virtual error onCullingVisible(Culler* culler, bool noCull) override;

        void ownerManagementNode(const SpatialId& id);
        const std::optional<SpatialId>& ownerManagementNode() const;

    protected:
        std::optional<SpatialId> m_ownerManagementId;
        bool m_hasTraversed;
    };
}

#endif // OUT_REGION_NODE_H
