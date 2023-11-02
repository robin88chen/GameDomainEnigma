#include "GameSceneService.h"

#include <execution>

#include "GameCameraService.h"
#include "SceneGraph/SceneGraphRepository.h"
#include "SceneGraph/Node.h"
#include "SceneGraph/PortalManagementNode.h"
#include "SceneGraph/FindSpatialByName.h"
#include "Platforms/MemoryMacro.h"
#include "Frameworks/EventPublisher.h"
#include "Frameworks/CommandBus.h"
#include "GameCameraEvents.h"
#include "GameSceneEvents.h"
#include "GameSceneCommands.h"
#include "GameCommonErrors.h"

using namespace Enigma::GameCommon;
using namespace Enigma::Frameworks;
using namespace Enigma::SceneGraph;
using namespace Enigma::MathLib;

DEFINE_RTTI(GameCommon, GameSceneService, ISystemService);

GameSceneService::GameSceneService(ServiceManager* mngr, const std::shared_ptr<SceneGraphRepository>& scene_graph_repository,
    const std::shared_ptr<GameCameraService>& camera_service) : ISystemService(mngr)
{
    m_sceneGraphRepository = scene_graph_repository;
    m_cameraService = camera_service;
    m_needTick = true;
    m_culler = nullptr;
}

GameSceneService::~GameSceneService()
{
    assert(!m_culler);
}

ServiceResult GameSceneService::onInit()
{
    m_onCameraCreated = std::make_shared<EventSubscriber>([=](auto e) { OnGameCameraCreated(e); });
    EventPublisher::subscribe(typeid(GameCameraCreated), m_onCameraCreated);
    m_onCameraUpdated = std::make_shared<EventSubscriber>([=](auto e) { OnGameCameraUpdated(e); });
    EventPublisher::subscribe(typeid(GameCameraUpdated), m_onCameraUpdated);

    m_doAttachingSceneRootChild = std::make_shared<CommandSubscriber>([=](auto c) { DoAttachingSceneRootChild(c); });
    CommandBus::subscribe(typeid(AttachSceneRootChild), m_doAttachingSceneRootChild);
    m_doAttachingNodeChild = std::make_shared<CommandSubscriber>([=](auto c) { DoAttachingNodeChild(c); });
    CommandBus::subscribe(typeid(AttachNodeChild), m_doAttachingNodeChild);

    return ServiceResult::Complete;
}

ServiceResult GameSceneService::onTick()
{
    if (m_culler)
    {
        m_culler->ComputeVisibleSet(GetSceneRoot());
    }

    return ServiceResult::Pendding;
}

ServiceResult GameSceneService::onTerm()
{
    EventPublisher::unsubscribe(typeid(GameCameraCreated), m_onCameraCreated);
    m_onCameraCreated = nullptr;
    EventPublisher::unsubscribe(typeid(GameCameraUpdated), m_onCameraUpdated);
    m_onCameraUpdated = nullptr;
    CommandBus::unsubscribe(typeid(AttachSceneRootChild), m_doAttachingSceneRootChild);
    m_doAttachingSceneRootChild = nullptr;
    CommandBus::unsubscribe(typeid(AttachNodeChild), m_doAttachingNodeChild);
    m_doAttachingNodeChild = nullptr;

    DestroyRootScene();
    DestroySceneCuller();

    return ServiceResult::Complete;
}

void GameSceneService::CreateRootScene(const std::string& scene_root_name, const std::optional<std::string>& portal_managed_name)
{
    assert(!m_sceneGraphRepository.expired());
    m_sceneRoot = m_sceneGraphRepository.lock()->CreateNode(scene_root_name, Node::TYPE_RTTI);
    m_sceneRoot->setLocalTransform(Matrix4::IDENTITY);

    if (portal_managed_name.has_value())
    {
        m_portalMgtNode = std::dynamic_pointer_cast<PortalManagementNode, Node>(
            m_sceneGraphRepository.lock()->CreateNode(portal_managed_name.value(), PortalManagementNode::TYPE_RTTI));
        m_sceneRoot->AttachChild(m_portalMgtNode, Matrix4::IDENTITY);
    }

    if ((!m_cameraService.expired()) && m_cameraService.lock()->GetPrimaryCamera())
    {
        CreateSceneCuller(m_cameraService.lock()->GetPrimaryCamera());
    }
    EventPublisher::post(std::make_shared<SceneRootCreated>(m_sceneRoot));
    EventPublisher::post(std::make_shared<PortalManagementNodeCreated>(m_portalMgtNode));
}

void GameSceneService::DestroyRootScene()
{
    m_sceneRoot = nullptr;
    m_portalMgtNode = nullptr;
}

error GameSceneService::AttachOutsideZone(const std::shared_ptr<SceneGraph::PortalZoneNode>& node)
{
    if (!m_portalMgtNode) return ErrorCode::nullPortalManagement;
    m_portalMgtNode->AttachOutsideZone(node);
    return ErrorCode::ok;
}

std::shared_ptr<Spatial> GameSceneService::FindSpatialByName(const std::string& spatial_name)
{
    if (!GetSceneRoot()) return nullptr;

    SceneGraph::FindSpatialByName finder(spatial_name);
    SceneTraveler::TravelResult res = GetSceneRoot()->visitBy(&finder);
    if (res == SceneTraveler::TravelResult::InterruptTargetFound)
    {
        return finder.GetFoundSpatial();
    }
    return nullptr;
}

void GameSceneService::CreateSceneCuller(const std::shared_ptr<Camera>& camera)
{
    SAFE_DELETE(m_culler);
    m_culler = menew Culler(camera);
    m_culler->EnableOuterClipping(true);
}

void GameSceneService::DestroySceneCuller()
{
    SAFE_DELETE(m_culler);
}

void GameSceneService::OnGameCameraCreated(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<GameCameraCreated, IEvent>(e);
    if (!ev) return;
    if (!ev->GetCamera()) return;
    CreateSceneCuller(ev->GetCamera());
}

void GameSceneService::OnGameCameraUpdated(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<GameCameraUpdated, IEvent>(e);
    if (!ev) return;
    if (!ev->GetCamera()) return;
    if (!m_culler) return;
    if (m_culler->GetCamera() == ev->GetCamera())
    {
        m_culler->UpdateFrustumPlanes();
    }
}

void GameSceneService::DoAttachingSceneRootChild(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<AttachSceneRootChild, ICommand>(c);
    if (!cmd) return;
    if (!cmd->GetChild()) return;
    if (!m_sceneRoot)
    {
        EventPublisher::post(std::make_shared<AttachSceneRootChildFailed>(cmd->GetChild()->getSpatialName(), ErrorCode::nullSceneRoot));
        return;
    }
    if (error er = m_sceneRoot->AttachChild(cmd->GetChild(), cmd->getLocalTransform()))
    {
        EventPublisher::post(std::make_shared<AttachSceneRootChildFailed>(cmd->GetChild()->getSpatialName(), er));
    }
    else
    {
        EventPublisher::post(std::make_shared<SceneRootChildAttached>(cmd->GetChild()));
    }
}

void GameSceneService::DoAttachingNodeChild(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<AttachNodeChild, ICommand>(c);
    if (!cmd) return;
    if (!cmd->GetChild()) return;
    auto node = std::dynamic_pointer_cast<Node, Spatial>(FindSpatialByName(cmd->GetNodeName()));
    if (!node)
    {
        EventPublisher::post(std::make_shared<AttachSceneNodeChildFailed>(cmd->GetNodeName(), cmd->GetChild()->getSpatialName(), ErrorCode::nodeNotFound));
        return;
    }
    if (error er = node->AttachChild(cmd->GetChild(), cmd->getLocalTransform()))
    {
        EventPublisher::post(std::make_shared<AttachSceneNodeChildFailed>(cmd->GetNodeName(), cmd->GetChild()->getSpatialName(), er));
    }
    else
    {
        EventPublisher::post(std::make_shared<SceneNodeChildAttached>(cmd->GetNodeName(), cmd->GetChild()));
    }
}

void GameSceneService::DoDeletingSceneSpatial(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<DeleteSceneSpatial, ICommand>(c);
    if (!cmd) return;
    auto spatial = FindSpatialByName(cmd->getName());
    if (!spatial)
    {
        EventPublisher::post(std::make_shared<DeleteSceneSpatialFailed>(cmd->getName(), ErrorCode::spatialNotFound));
        return;
    }
    if (const auto parent = spatial->getParent())
    {
        if (const auto parent_node = std::dynamic_pointer_cast<Node, Spatial>(parent))
        {
            parent_node->DetachChild(spatial);
        }
    }
    spatial = nullptr;
    EventPublisher::post(std::make_shared<SceneSpatialDeleted>(cmd->getName()));
}
