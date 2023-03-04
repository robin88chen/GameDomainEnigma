/*********************************************************************
 * \file   LazyNodeIOService.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   February 2023
 *********************************************************************/
#ifndef _LAZY_NODE_IO_SERVICE_H
#define _LAZY_NODE_IO_SERVICE_H

#include "Frameworks/SystemService.h"
#include "Frameworks/CommandSubscriber.h"
#include "GameEngine/DtoDeserializer.h"
#include "Frameworks/EventSubscriber.h"
#include "GameEngine/TimerService.h"
#include <memory>
#include <deque>
#include <mutex>
#include <unordered_map>

namespace Enigma::SceneGraph
{
    class LazyNode;
    class LazyNodeIOService : public Frameworks::ISystemService
    {
        DECLARE_EN_RTTI;
    public:
        LazyNodeIOService(Frameworks::ServiceManager* mngr, Engine::TimerService* timer, const std::shared_ptr<Engine::IDtoDeserializer>& dto_deserializer);
        LazyNodeIOService(const LazyNodeIOService&) = delete;
        LazyNodeIOService(LazyNodeIOService&&) = delete;
        virtual ~LazyNodeIOService() override;
        LazyNodeIOService& operator=(const LazyNodeIOService&) = delete;
        LazyNodeIOService& operator=(LazyNodeIOService&&) = delete;

        virtual Frameworks::ServiceResult OnInit() override;
        virtual Frameworks::ServiceResult OnTick() override;
        virtual Frameworks::ServiceResult OnTerm() override;

    private:
        void DoInstancingLazyNode(const Frameworks::ICommandPtr& c);
        void OnDtoDeserialized(const Frameworks::IEventPtr& e);
        void OnDeserializingDtoFailed(const Frameworks::IEventPtr& e);
        void OnInPlaceSceneGraphBuilt(const Frameworks::IEventPtr& e);
        void OnVisibilityChanged(const Frameworks::IEventPtr& e);

        void InstanceNextLazyNode();

    private:
        Engine::TimerService* m_timer;

        Frameworks::CommandSubscriberPtr m_doInstancingLazyNode;
        std::shared_ptr<Engine::IDtoDeserializer> m_dtoDeserializer;

        Frameworks::Ruid m_ruidDeserializing;
        std::shared_ptr<LazyNode> m_in_placeNode;

        Frameworks::EventSubscriberPtr m_onDtoDeserialized;
        Frameworks::EventSubscriberPtr m_onDeserializingDtoFailed;
        Frameworks::EventSubscriberPtr m_onInPlaceSceneGraphBuilt;
        Frameworks::EventSubscriberPtr m_onVisibilityChanged;

        std::deque<std::shared_ptr<LazyNode>> m_waitingNodes;
        std::mutex m_waitingNodesLock;
        std::atomic_bool m_isCurrentInstancing;

        std::unordered_map<std::shared_ptr<LazyNode>, float> m_visibilityTimers;
    };
}

#endif // _LAZY_NODE_IO_SERVICE_H
