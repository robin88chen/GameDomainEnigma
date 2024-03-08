/*********************************************************************
 * \file   LazyNodeHydrationService.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   February 2023
 *********************************************************************/
#ifndef _LAZY_NODE_HYDRATION_SERVICE_H
#define _LAZY_NODE_HYDRATION_SERVICE_H

#include "Frameworks/SystemService.h"
#include "Frameworks/CommandSubscriber.h"
#include "Frameworks/EventSubscriber.h"
#include "GameEngine/TimerService.h"
#include "SpatialId.h"
#include <memory>
#include <deque>
#include <mutex>
#include <unordered_map>

namespace Enigma::SceneGraph
{
    class SceneGraphRepository;
    class LazyNode;
    class LazyNodeHydrationService : public Frameworks::ISystemService
    {
        DECLARE_EN_RTTI;
    public:
        LazyNodeHydrationService(Frameworks::ServiceManager* mngr, const std::shared_ptr<SceneGraphRepository>& scene_graph_repository, const std::shared_ptr<Engine::TimerService>& timer);
        LazyNodeHydrationService(const LazyNodeHydrationService&) = delete;
        LazyNodeHydrationService(LazyNodeHydrationService&&) = delete;
        virtual ~LazyNodeHydrationService() override;
        LazyNodeHydrationService& operator=(const LazyNodeHydrationService&) = delete;
        LazyNodeHydrationService& operator=(LazyNodeHydrationService&&) = delete;

        virtual Frameworks::ServiceResult onInit() override;
        virtual Frameworks::ServiceResult onTick() override;
        virtual Frameworks::ServiceResult onTerm() override;

    private:
        void registerHandlers();
        void unregisterHandlers();

        void hydrateLazyNode(const Frameworks::ICommandPtr& c);
        void onVisibilityChanged(const Frameworks::IEventPtr& e);

        void hydrateNextLazyNode();

    private:
        std::weak_ptr<SceneGraphRepository> m_sceneGraphRepository;
        std::weak_ptr<Engine::TimerService> m_timer;

        Frameworks::CommandSubscriberPtr m_hydrateLazyNode;

        SpatialId m_hydratingId;

        Frameworks::EventSubscriberPtr m_onVisibilityChanged;

        std::deque<SpatialId> m_waitingNodes;
        std::mutex m_waitingNodesLock;
        std::atomic_bool m_isCurrentHydrating;

        std::unordered_map<SpatialId, float, SpatialId::hash> m_visibilityTimers;
    };
}

#endif // _LAZY_NODE_HYDRATION_SERVICE_H
