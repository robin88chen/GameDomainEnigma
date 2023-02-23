#include "LazyNodeIOService.h"
#include "Frameworks/CommandBus.h"
#include "Frameworks/EventPublisher.h"
#include "SceneGraphCommands.h"
#include "SceneGraphEvents.h"
#include "GameEngine/DtoDeserializer.h"
#include "GameEngine/DtoEvents.h"
#include "LazyNode.h"
#include "Platforms/PlatformLayerUtilities.h"

using namespace Enigma::SceneGraph;
using namespace Enigma::Frameworks;
using namespace Enigma::Engine;

DEFINE_RTTI(SceneGraph, LazyNodeIOService, ISystemService);

LazyNodeIOService::LazyNodeIOService(Frameworks::ServiceManager* mngr,
    const std::shared_ptr<Engine::IDtoDeserializer>& dto_deserializer) : ISystemService(mngr)
{
    m_dtoDeserializer = dto_deserializer;
    m_needTick = false;
    m_isCurrentInstancing = false;
    m_doInstancingLazyNode = std::make_shared<CommandSubscriber>([=](auto c) { DoInstancingLazyNode(c); });
    CommandBus::Subscribe(typeid(InstanceLazyNode), m_doInstancingLazyNode);

    m_onDtoDeserialized = std::make_shared<EventSubscriber>([=](auto e) { OnDtoDeserialized(e); });
    m_onDeserializingDtoFailed = std::make_shared<EventSubscriber>([=](auto e) { OnDeserializingDtoFailed(e); });
    m_onInPlaceSceneGraphBuilt = std::make_shared<EventSubscriber>([=](auto e) { OnInPlaceSceneGraphBuilt(e); });
    EventPublisher::Subscribe(typeid(GenericDtoDeserialized), m_onDtoDeserialized);
    EventPublisher::Subscribe(typeid(DeserializeDtoFailed), m_onDeserializingDtoFailed);
    EventPublisher::Subscribe(typeid(InPlaceSceneGraphBuilt), m_onInPlaceSceneGraphBuilt);
}

LazyNodeIOService::~LazyNodeIOService()
{
    EventPublisher::Unsubscribe(typeid(GenericDtoDeserialized), m_onDtoDeserialized);
    EventPublisher::Unsubscribe(typeid(DeserializeDtoFailed), m_onDeserializingDtoFailed);
    EventPublisher::Subscribe(typeid(InPlaceSceneGraphBuilt), m_onInPlaceSceneGraphBuilt);
    m_onDtoDeserialized = nullptr;
    m_onDeserializingDtoFailed = nullptr;
    m_onInPlaceSceneGraphBuilt = nullptr;

    CommandBus::Unsubscribe(typeid(InstanceLazyNode), m_doInstancingLazyNode);
    m_doInstancingLazyNode = nullptr;
}

ServiceResult LazyNodeIOService::OnTick()
{
    if (m_isCurrentInstancing) return ServiceResult::Pendding;
    InstanceNextLazyNode();
    return ServiceResult::Pendding;
}

void LazyNodeIOService::DoInstancingLazyNode(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    std::lock_guard locker{ m_InstanceCommandsLock };
    m_InstanceCommands.push_back(c);
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
    EventPublisher::Post(std::make_shared<LazyNodeInstanced>(m_in_placeNode));
    m_isCurrentInstancing = false;
}

void LazyNodeIOService::InstanceNextLazyNode()
{
    if (m_isCurrentInstancing) return;
    if (m_InstanceCommands.empty())
    {
        m_needTick = false;
        return;
    }

    std::lock_guard locker{ m_InstanceCommandsLock };
    auto c = m_InstanceCommands.front();
    m_InstanceCommands.pop_front();
    const auto cmd = std::dynamic_pointer_cast<InstanceLazyNode, ICommand>(c);
    if (!cmd) return;
    if (!cmd->GetNode()) return;
    if (!cmd->GetNode()->TheLazyStatus().IsGhost()) return;
    m_in_placeNode = cmd->GetNode();
    m_in_placeNode->TheLazyStatus().ChangeStatus(LazyStatus::Status::Loading);
    m_isCurrentInstancing = true;
    if (m_dtoDeserializer)
    {
        m_ruidDeserializing = Ruid::Generate();
        m_dtoDeserializer->InvokeDeserialize(m_ruidDeserializing, cmd->GetNode()->TheFactoryDesc().GetDeferredFilename());
    }
    else
    {
        Platforms::Debug::ErrorPrintf("Instance Lazy Node without dto deserializer!!");
        m_in_placeNode->TheLazyStatus().ChangeStatus(LazyStatus::Status::Ghost);
        m_isCurrentInstancing = false;
    }
}
