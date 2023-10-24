#include "LazyNodeIOService.h"
#include "Frameworks/CommandBus.h"
#include "Frameworks/EventPublisher.h"
#include "SceneGraphCommands.h"
#include "SceneGraphEvents.h"
#include "GameEngine/DtoDeserializer.h"
#include "GameEngine/DtoEvents.h"
#include "LazyNode.h"
#include "GameEngine/TimerService.h"
#include "Platforms/PlatformLayerUtilities.h"

using namespace Enigma::SceneGraph;
using namespace Enigma::Frameworks;
using namespace Enigma::Engine;

float constexpr MIN_KEEP_TIME = 5.0000f;

DEFINE_RTTI(SceneGraph, LazyNodeIOService, ISystemService);

LazyNodeIOService::LazyNodeIOService(Frameworks::ServiceManager* mngr, const std::shared_ptr<Engine::TimerService>& timer,
    const std::shared_ptr<Engine::IDtoDeserializer>& dto_deserializer) : ISystemService(mngr), m_timer(timer), m_dtoDeserializer(dto_deserializer), m_isCurrentInstancing(false), m_ruidDeserializing()
{
    m_needTick = false;
}

LazyNodeIOService::~LazyNodeIOService()
{
}

ServiceResult LazyNodeIOService::onInit()
{
    m_doInstancingLazyNode = std::make_shared<CommandSubscriber>([=](auto c) { DoInstancingLazyNode(c); });
    CommandBus::Subscribe(typeid(InstanceLazyNode), m_doInstancingLazyNode);

    m_onDtoDeserialized = std::make_shared<EventSubscriber>([=](auto e) { OnDtoDeserialized(e); });
    m_onDeserializingDtoFailed = std::make_shared<EventSubscriber>([=](auto e) { OnDeserializingDtoFailed(e); });
    m_onInPlaceSceneGraphBuilt = std::make_shared<EventSubscriber>([=](auto e) { OnInPlaceSceneGraphBuilt(e); });
    m_onVisibilityChanged = std::make_shared<EventSubscriber>([=](auto e) { OnVisibilityChanged(e); });
    EventPublisher::Subscribe(typeid(GenericDtoDeserialized), m_onDtoDeserialized);
    EventPublisher::Subscribe(typeid(DeserializeDtoFailed), m_onDeserializingDtoFailed);
    EventPublisher::Subscribe(typeid(InPlaceSceneGraphBuilt), m_onInPlaceSceneGraphBuilt);
    EventPublisher::Subscribe(typeid(VisibilityChanged), m_onVisibilityChanged);

    return ServiceResult::Complete;
}

ServiceResult LazyNodeIOService::onTick()
{
    if (m_isCurrentInstancing) return ServiceResult::Pendding;
    InstanceNextLazyNode();
    return ServiceResult::Pendding;
}

ServiceResult LazyNodeIOService::onTerm()
{
    m_in_placeNode = nullptr;
    {
        std::lock_guard locker{ m_waitingNodesLock };
        m_waitingNodes.clear();
    }
    m_visibilityTimers.clear();

    EventPublisher::Unsubscribe(typeid(GenericDtoDeserialized), m_onDtoDeserialized);
    EventPublisher::Unsubscribe(typeid(DeserializeDtoFailed), m_onDeserializingDtoFailed);
    EventPublisher::Unsubscribe(typeid(InPlaceSceneGraphBuilt), m_onInPlaceSceneGraphBuilt);
    EventPublisher::Unsubscribe(typeid(VisibilityChanged), m_onVisibilityChanged);
    m_onDtoDeserialized = nullptr;
    m_onDeserializingDtoFailed = nullptr;
    m_onInPlaceSceneGraphBuilt = nullptr;
    m_onVisibilityChanged = nullptr;

    CommandBus::Unsubscribe(typeid(InstanceLazyNode), m_doInstancingLazyNode);
    m_doInstancingLazyNode = nullptr;

    return ServiceResult::Complete;
}

void LazyNodeIOService::DoInstancingLazyNode(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<InstanceLazyNode, ICommand>(c);
    if (!cmd) return;
    if (!cmd->GetNode()) return;
    if (!cmd->GetNode()->TheLazyStatus().IsGhost()) return;
    std::lock_guard locker{ m_waitingNodesLock };
    cmd->GetNode()->TheLazyStatus().ChangeStatus(LazyStatus::Status::InQueue);
    m_waitingNodes.push_back(cmd->GetNode());
    m_needTick = true;
}

void LazyNodeIOService::OnDtoDeserialized(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<GenericDtoDeserialized, IEvent>(e);
    if (!ev) return;
    if (ev->GetRuid() != m_ruidDeserializing) return;
    CommandBus::Post(std::make_shared<InPlaceBuildSceneGraph>(m_in_placeNode, ev->GetDtos()));
}

void LazyNodeIOService::OnDeserializingDtoFailed(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<DeserializeDtoFailed, IEvent>(e);
    if (!ev) return;
    if (ev->GetRuid() != m_ruidDeserializing) return;
    std::string node_name = "unknown";
    if (m_in_placeNode) node_name = m_in_placeNode->GetSpatialName();
    Platforms::Debug::ErrorPrintf("deserializing lazy node %s dto failed : %s", node_name.c_str(), ev->GetErrorCode().message().c_str());
    EventPublisher::Post(std::make_shared<InstanceLazyNodeFailed>(m_in_placeNode, ev->GetErrorCode()));
    if (m_in_placeNode) m_in_placeNode->TheLazyStatus().ChangeStatus(LazyStatus::Status::Ghost);
    m_isCurrentInstancing = false;
}

void LazyNodeIOService::OnInPlaceSceneGraphBuilt(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<InPlaceSceneGraphBuilt, IEvent>(e);
    if (!ev) return;
    if (!m_in_placeNode) return;
    if (ev->GetInPlaceRootNode() != m_in_placeNode) return;
    m_in_placeNode->TheLazyStatus().ChangeStatus(LazyStatus::Status::Ready);
    m_in_placeNode->TheFactoryDesc().ClaimAsInstanced();
    EventPublisher::Post(std::make_shared<LazyNodeInstanced>(m_in_placeNode));
    m_isCurrentInstancing = false;
}

void LazyNodeIOService::OnVisibilityChanged(const Frameworks::IEventPtr& e)
{
    assert(!m_timer.expired());
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<VisibilityChanged, IEvent>(e);
    if (!ev) return;
    if (!ev->GetNode()) return;
    if (ev->IsVisible())
    {
        m_visibilityTimers.insert_or_assign(ev->GetNode(), m_timer.lock()->GetGameTimer()->GetTotalTime());
    }
    else
    {
        auto it_vis = m_visibilityTimers.find(ev->GetNode());
        if (it_vis != m_visibilityTimers.end())
        {
            auto time = m_timer.lock()->GetGameTimer()->GetTotalTime() - it_vis->second;
            if ((time > MIN_KEEP_TIME) && (m_in_placeNode != ev->GetNode()))
            {
                std::lock_guard locker{ m_waitingNodesLock };
                auto it = std::find(m_waitingNodes.begin(), m_waitingNodes.end(), ev->GetNode());
                if (it != m_waitingNodes.end()) m_waitingNodes.erase(it);
                m_visibilityTimers.erase(ev->GetNode());
            }
        }
    }
}

void LazyNodeIOService::InstanceNextLazyNode()
{
    if (m_isCurrentInstancing) return;
    if (m_waitingNodes.empty())
    {
        m_needTick = false;
        return;
    }

    std::lock_guard locker{ m_waitingNodesLock };
    const auto node = m_waitingNodes.front();
    m_waitingNodes.pop_front();
    m_in_placeNode = node;
    m_in_placeNode->TheLazyStatus().ChangeStatus(LazyStatus::Status::Loading);
    m_isCurrentInstancing = true;
    if (m_dtoDeserializer)
    {
        m_ruidDeserializing = Ruid::Generate();
        m_dtoDeserializer->InvokeDeserialize(m_ruidDeserializing, node->TheFactoryDesc().GetDeferredFilename());
    }
    else
    {
        Platforms::Debug::ErrorPrintf("Instance Lazy Node without dto deserializer!!");
        m_in_placeNode->TheLazyStatus().ChangeStatus(LazyStatus::Status::Ghost);
        m_isCurrentInstancing = false;
    }
}
