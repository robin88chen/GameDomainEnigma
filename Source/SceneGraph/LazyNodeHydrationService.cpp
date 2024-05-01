#include "LazyNodeHydrationService.h"
#include "Frameworks/CommandBus.h"
#include "Frameworks/EventPublisher.h"
#include "SceneGraphCommands.h"
#include "SceneGraphEvents.h"
#include "LazyNode.h"
#include "SceneGraphRepository.h"
#include "GameEngine/TimerService.h"
#include "Platforms/PlatformLayerUtilities.h"

using namespace Enigma::SceneGraph;
using namespace Enigma::Frameworks;
using namespace Enigma::Engine;

float constexpr MIN_KEEP_TIME = 5.0000f;

DEFINE_RTTI(SceneGraph, LazyNodeHydrationService, ISystemService);

LazyNodeHydrationService::LazyNodeHydrationService(Frameworks::ServiceManager* mngr, const std::shared_ptr<SceneGraphRepository>& scene_graph_repository, const std::shared_ptr<Engine::TimerService>& timer) : ISystemService(mngr), m_sceneGraphRepository(scene_graph_repository), m_timer(timer), m_isCurrentHydrating(false)
{
    m_needTick = false;
    registerHandlers();
}

LazyNodeHydrationService::~LazyNodeHydrationService()
{
    unregisterHandlers();
}

ServiceResult LazyNodeHydrationService::onInit()
{
    return ServiceResult::Complete;
}

ServiceResult LazyNodeHydrationService::onTick()
{
    if (m_isCurrentHydrating) return ServiceResult::Pendding;
    hydrateNextLazyNode();
    return ServiceResult::Pendding;
}

ServiceResult LazyNodeHydrationService::onTerm()
{
    {
        std::lock_guard locker{ m_waitingNodesLock };
        m_waitingNodes.clear();
    }
    m_visibilityTimers.clear();

    return ServiceResult::Complete;
}

void LazyNodeHydrationService::registerHandlers()
{
    m_hydrateLazyNode = std::make_shared<CommandSubscriber>([=](auto c) { hydrateLazyNode(c); });
    CommandBus::subscribe(typeid(HydrateLazyNode), m_hydrateLazyNode);

    m_onLazyNodeHydrated = std::make_shared<EventSubscriber>([=](auto e) { onLazyNodeHydrated(e); });
    EventPublisher::subscribe(typeid(LazyNodeHydrated), m_onLazyNodeHydrated);
    m_onLazyNodeHydrationFailed = std::make_shared<EventSubscriber>([=](auto e) { onLazyNodeHydrationFailed(e); });
    EventPublisher::subscribe(typeid(LazyNodeHydrationFailed), m_onLazyNodeHydrationFailed);
    m_onVisibilityChanged = std::make_shared<EventSubscriber>([=](auto e) { onVisibilityChanged(e); });
    EventPublisher::subscribe(typeid(VisibilityChanged), m_onVisibilityChanged);
}

void LazyNodeHydrationService::unregisterHandlers()
{
    EventPublisher::unsubscribe(typeid(LazyNodeHydrated), m_onLazyNodeHydrated);
    m_onLazyNodeHydrated = nullptr;
    EventPublisher::unsubscribe(typeid(LazyNodeHydrationFailed), m_onLazyNodeHydrationFailed);
    m_onLazyNodeHydrationFailed = nullptr;
    EventPublisher::unsubscribe(typeid(VisibilityChanged), m_onVisibilityChanged);
    m_onVisibilityChanged = nullptr;

    CommandBus::unsubscribe(typeid(HydrateLazyNode), m_hydrateLazyNode);
    m_hydrateLazyNode = nullptr;
}

void LazyNodeHydrationService::hydrateLazyNode(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<HydrateLazyNode>(c);
    if (!cmd) return;
    auto lazy_node = std::dynamic_pointer_cast<LazyNode>(Node::queryNode(cmd->id()));
    if (!lazy_node) return;
    if (!lazy_node->lazyStatus().isGhost()) return;
    std::lock_guard locker{ m_waitingNodesLock };
    lazy_node->lazyStatus().changeStatus(LazyStatus::Status::InQueue);
    m_waitingNodes.push_back(cmd->id());
    m_needTick = true;
}

void LazyNodeHydrationService::onLazyNodeHydrated(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<LazyNodeHydrated, IEvent>(e);
    if (!ev) return;
    if (ev->id().empty()) return;
    m_isCurrentHydrating = false;
    m_hydratingId = SpatialId();
    hydrateNextLazyNode();
}

void LazyNodeHydrationService::onLazyNodeHydrationFailed(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<LazyNodeHydrationFailed, IEvent>(e);
    if (!ev) return;
    if (ev->id().empty()) return;
    Platforms::Debug::ErrorPrintf("Lazy Node %s(%s) hydration failed : %s", ev->id().name().c_str(), ev->id().rtti().getName().c_str(), ev->error().message().c_str());
    m_isCurrentHydrating = false;
    m_hydratingId = SpatialId();
    hydrateNextLazyNode();
}

void LazyNodeHydrationService::onVisibilityChanged(const Frameworks::IEventPtr& e)
{
    assert(!m_timer.expired());
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<VisibilityChanged, IEvent>(e);
    if (!ev) return;
    if (ev->id().empty()) return;
    if (ev->isVisible())
    {
        m_visibilityTimers.insert_or_assign(ev->id(), m_timer.lock()->GetGameTimer()->getTotalTime());
    }
    else
    {
        auto it_vis = m_visibilityTimers.find(ev->id());
        if (it_vis != m_visibilityTimers.end())
        {
            auto time = m_timer.lock()->GetGameTimer()->getTotalTime() - it_vis->second;
            if ((time > MIN_KEEP_TIME) && (m_hydratingId != ev->id()))
            {
                std::lock_guard locker{ m_waitingNodesLock };
                auto it = std::find(m_waitingNodes.begin(), m_waitingNodes.end(), ev->id());
                if (it != m_waitingNodes.end()) m_waitingNodes.erase(it);
                m_visibilityTimers.erase(ev->id());
            }
        }
    }
}

void LazyNodeHydrationService::hydrateNextLazyNode()
{
    assert(!m_sceneGraphRepository.expired());
    if (m_isCurrentHydrating) return;
    if (m_waitingNodes.empty())
    {
        m_needTick = false;
        return;
    }

    std::lock_guard locker{ m_waitingNodesLock };
    const auto id = m_waitingNodes.front();
    m_waitingNodes.pop_front();
    m_hydratingId = id;
    auto lazy_node = std::dynamic_pointer_cast<LazyNode>(Node::queryNode(id));
    if (!lazy_node)
    {
        m_isCurrentHydrating = false;
        return;
    }
    m_sceneGraphRepository.lock()->hydrateLazyNode(m_hydratingId);
    m_isCurrentHydrating = true;
}
