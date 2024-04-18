/*****************************************************************
 * \file   NodalSceneGraph.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   April 2024
 ******************************************************************/
#ifndef NODAL_SCENE_GRAPH_H
#define NODAL_SCENE_GRAPH_H

#include "SceneGraph.h"
#include <memory>

namespace Enigma::SceneGraph
{
    class NodalSceneGraph : public SceneGraph
    {
    public:
        NodalSceneGraph(const std::shared_ptr<SceneGraphRepository>& scene_graph_repository);
        NodalSceneGraph(const NodalSceneGraph& other) = delete;
        NodalSceneGraph(NodalSceneGraph&& other) noexcept = delete;
        ~NodalSceneGraph() override;
        NodalSceneGraph& operator=(const NodalSceneGraph& other) = delete;
        NodalSceneGraph& operator=(NodalSceneGraph&& other) noexcept = delete;

        virtual error createRoot(const SpatialId& scene_root_id) override;
        virtual error destroyRoot() override;
        virtual std::shared_ptr<Node> root() const override { return m_root; };

    protected:
        std::shared_ptr<Node> m_root;
    };
}

#endif // NODAL_SCENE_GRAPH_H
