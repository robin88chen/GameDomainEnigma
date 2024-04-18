/*****************************************************************
 * \file   SceneGraph.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   April 2024
 ******************************************************************/
#ifndef SCENE_GRAPHI_H
#define SCENE_GRAPHI_H

#include "Frameworks/CommandSubscriber.h"
#include <memory>
#include <system_error>

namespace Enigma::SceneGraph
{
    using error = std::error_code;
    class SceneGraphRepository;
    class Node;
    class Spatial;
    class SpatialId;

    class SceneGraph
    {
    public:
        SceneGraph(const std::shared_ptr<SceneGraphRepository>& scene_graph_repository);
        SceneGraph(const SceneGraph& other) = delete;
        SceneGraph(SceneGraph&& other) noexcept = delete;
        virtual ~SceneGraph();
        SceneGraph& operator=(const SceneGraph& other) = delete;
        SceneGraph& operator=(SceneGraph&& other) noexcept = delete;

        virtual void registerHandlers();
        virtual void unregisterHandlers();

        virtual error createRoot(const SpatialId& scene_root_id) = 0;
        virtual error destroyRoot() = 0;

        // 不能傳回 const 參考，會有轉型問題
        virtual std::shared_ptr<Node> root() const = 0;

    protected:
        std::shared_ptr<Spatial> findSpatial(const SpatialId& spatial_id);

        void attachNodeChild(const Frameworks::ICommandPtr& c);
        void detachNodeChild(const Frameworks::ICommandPtr& c);
        void deleteSceneSpatial(const Frameworks::ICommandPtr& c);

    protected:
        std::weak_ptr<SceneGraphRepository> m_sceneGraphRepository;

        Frameworks::CommandSubscriberPtr m_attachNodeChild;
        Frameworks::CommandSubscriberPtr m_detachNodeChild;
        Frameworks::CommandSubscriberPtr m_deleteSceneSpatial;
    };
}

#endif // SCENE_GRAPHI_NTERFACE_H
