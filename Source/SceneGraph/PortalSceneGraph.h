/*****************************************************************
 * \file   PortalSceneGraph.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   April 2024
 ******************************************************************/
#ifndef PORTAL_SCENE_GRAPH_H
#define PORTAL_SCENE_GRAPH_H

#include "SceneGraph.h"
#include "PortalManagementNode.h"
#include <memory>

namespace Enigma::SceneGraph
{
    class PortalSceneGraph : public SceneGraph
    {
    public:
        PortalSceneGraph(const std::shared_ptr<SceneGraphRepository>& scene_graph_repository);
        PortalSceneGraph(const PortalSceneGraph& other) = delete;
        PortalSceneGraph(PortalSceneGraph&& other) noexcept = delete;
        ~PortalSceneGraph() override;
        PortalSceneGraph& operator=(const PortalSceneGraph& other) = delete;
        PortalSceneGraph& operator=(PortalSceneGraph&& other) noexcept = delete;

        virtual error createRoot(const SpatialId& scene_root_id) override;
        virtual error destroyRoot() override;
        virtual std::shared_ptr<Node> root() const override { return m_root; };

    protected:
        std::shared_ptr<PortalManagementNode> m_root;
    };
}


#endif // PORTAL_SCENE_GRAPH_H
