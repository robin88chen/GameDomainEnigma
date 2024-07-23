/*****************************************************************
 * \file   SceneGraphRepositoryPendingStash.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   July 2024
 ******************************************************************/
#ifndef SCENE_GRAPH_REPOSITORY_PENDING_STASH_H
#define SCENE_GRAPH_REPOSITORY_PENDING_STASH_H

#include "SpatialId.h"
#include "Frameworks/EventSubscriber.h"

namespace Enigma::SceneGraph
{
    class Spatial;

    class SceneGraphRepositoryPendingStash
    {
    public:
        SceneGraphRepositoryPendingStash();
        SceneGraphRepositoryPendingStash(const SceneGraphRepositoryPendingStash&) = delete;
        SceneGraphRepositoryPendingStash(SceneGraphRepositoryPendingStash&&) noexcept = delete;
        ~SceneGraphRepositoryPendingStash();
        SceneGraphRepositoryPendingStash& operator=(const SceneGraphRepositoryPendingStash&) = delete;
        SceneGraphRepositoryPendingStash& operator=(SceneGraphRepositoryPendingStash&&) noexcept = delete;

        void registerHandlers();
        void unregisterHandlers();

        void removePendingSpatialAsSceneRoot(const SpatialId& id);
        void insertPendingSpatial(const SpatialId& id, const std::shared_ptr<Spatial>& spatial);

    protected:
        void onNodeChildAttached(const Frameworks::IEventPtr& e);
        void onNodeChildAttachmentFailed(const Frameworks::IEventPtr& e);

        void removeStashedSpatial(const SpatialId& id);

    protected:
        std::unordered_map<SpatialId, std::shared_ptr<Spatial>, SpatialId::hash> m_pendingSpatial;

        Frameworks::EventSubscriberPtr m_nodeChildAttached;
        Frameworks::EventSubscriberPtr m_nodeChildAttachmentFailed;
    };
}

#endif // SCENE_GRAPH_REPOSITORY_PENDING_STASH_H
