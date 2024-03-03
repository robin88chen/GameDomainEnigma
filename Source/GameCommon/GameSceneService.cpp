#include "GameSceneService.h"
#include "GameCameraService.h"
#include "SceneGraph/SceneGraphRepository.h"
#include "SceneGraph/Node.h"
#include "SceneGraph/PortalManagementNode.h"
#include "SceneGraph/FindSpatialById.h"
#include "Platforms/MemoryMacro.h"
#include "Frameworks/EventPublisher.h"
#include "Frameworks/CommandBus.h"
#include "GameCameraEvents.h"
#include "GameSceneEvents.h"
#include "GameSceneCommands.h"
#include "GameCommonErrors.h"
#include "SceneGraph/SceneGraphQueries.h"

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
    m_onCameraCreated = std::make_shared<EventSubscriber>([=](auto e) { onGameCameraCreated(e); });
    EventPublisher::subscribe(typeid(GameCameraCreated), m_onCameraCreated);
    m_onCameraUpdated = std::make_shared<EventSubscriber>([=](auto e) { onGameCameraUpdated(e); });
    EventPublisher::subscribe(typeid(GameCameraUpdated), m_onCameraUpdated);

    m_attachSceneRootChild = std::make_shared<CommandSubscriber>([=](auto c) { attachSceneRootChild(c); });
    CommandBus::subscribe(typeid(AttachSceneRootChild), m_attachSceneRootChild);

    return ServiceResult::Complete;
}

ServiceResult GameSceneService::onTick()
{
    if (m_culler)
    {
        m_culler->ComputeVisibleSet(getSceneRoot());
    }

    return ServiceResult::Pendding;
}

ServiceResult GameSceneService::onTerm()
{
    EventPublisher::unsubscribe(typeid(GameCameraCreated), m_onCameraCreated);
    m_onCameraCreated = nullptr;
    EventPublisher::unsubscribe(typeid(GameCameraUpdated), m_onCameraUpdated);
    m_onCameraUpdated = nullptr;
    CommandBus::unsubscribe(typeid(AttachSceneRootChild), m_attachSceneRootChild);
    m_attachSceneRootChild = nullptr;

    destroyRootScene();
    destroySceneCuller();

    return ServiceResult::Complete;
}

void GameSceneService::createRootScene(const SpatialId& scene_root_id, PersistenceLevel persistence_level, const std::optional<std::string>& portal_managed_name)
{
    assert(!m_sceneGraphRepository.expired());
    auto root = std::make_shared<QuerySpatial>(scene_root_id)->dispatch();
    if (!root)
    {
        root = std::make_shared<RequestSpatialCreation>(scene_root_id, persistence_level)->dispatch();
    }
    assert(root);
    m_sceneRoot = std::dynamic_pointer_cast<Node>(root);
    assert(m_sceneRoot);
    m_sceneRoot->setLocalTransform(Matrix4::IDENTITY);

    if (portal_managed_name.has_value())
    {
        m_portalMgtNode = std::dynamic_pointer_cast<PortalManagementNode, Node>(
            m_sceneGraphRepository.lock()->createNode(portal_managed_name.value(), Engine::FactoryDesc(PortalManagementNode::TYPE_RTTI.getName()).ClaimAsNative(portal_managed_name.value())));
        m_sceneRoot->attachChild(m_portalMgtNode, Matrix4::IDENTITY);
    }

    if ((!m_cameraService.expired()) && m_cameraService.lock()->primaryCamera())
    {
        createSceneCuller(m_cameraService.lock()->primaryCamera());
    }
    EventPublisher::post(std::make_shared<SceneRootCreated>(m_sceneRoot));
    EventPublisher::post(std::make_shared<PortalManagementNodeCreated>(m_portalMgtNode));
}

void GameSceneService::destroyRootScene()
{
    m_sceneRoot = nullptr;
    m_portalMgtNode = nullptr;
}

error GameSceneService::attachOutsideZone(const std::shared_ptr<SceneGraph::PortalZoneNode>& node)
{
    if (!m_portalMgtNode) return ErrorCode::nullPortalManagement;
    m_portalMgtNode->attachOutsideZone(node);
    return ErrorCode::ok;
}

std::shared_ptr<Spatial> GameSceneService::findSpatial(const SceneGraph::SpatialId& spatial_id)
{
    if (!getSceneRoot()) return nullptr;

    FindSpatialById finder(spatial_id);
    SceneTraveler::TravelResult res = getSceneRoot()->visitBy(&finder);
    if (res == SceneTraveler::TravelResult::InterruptTargetFound)
    {
        return finder.getFoundSpatial();
    }
    return nullptr;
}

void GameSceneService::createSceneCuller(const std::shared_ptr<Camera>& camera)
{
    SAFE_DELETE(m_culler);
    m_culler = menew Culler(camera);
    m_culler->EnableOuterClipping(true);
}

void GameSceneService::destroySceneCuller()
{
    SAFE_DELETE(m_culler);
}

void GameSceneService::onGameCameraCreated(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<GameCameraCreated, IEvent>(e);
    if (!ev) return;
    if (!ev->GetCamera()) return;
    createSceneCuller(ev->GetCamera());
}

void GameSceneService::onGameCameraUpdated(const IEventPtr& e)
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

void GameSceneService::attachSceneRootChild(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<AttachSceneRootChild, ICommand>(c);
    if (!cmd) return;
    if (!cmd->child()) return;
    if (!m_sceneRoot)
    {
        EventPublisher::post(std::make_shared<AttachSceneRootChildFailed>(cmd->child()->id(), ErrorCode::nullSceneRoot));
        return;
    }
    if (error er = m_sceneRoot->attachChild(cmd->child(), cmd->localTransform()))
    {
        EventPublisher::post(std::make_shared<AttachSceneRootChildFailed>(cmd->child()->id(), er));
    }
    else
    {
        EventPublisher::post(std::make_shared<SceneRootChildAttached>(cmd->child()));
    }
}

void GameSceneService::deleteSceneSpatial(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<DeleteSceneSpatial, ICommand>(c);
    if (!cmd) return;
    auto spatial = findSpatial(cmd->id());
    if (!spatial)
    {
        EventPublisher::post(std::make_shared<DeleteSceneSpatialFailed>(cmd->id(), ErrorCode::spatialNotFound));
        return;
    }
    if (const auto parent = spatial->getParent())
    {
        if (const auto parent_node = std::dynamic_pointer_cast<Node, Spatial>(parent))
        {
            parent_node->detachChild(spatial);
        }
    }
    spatial = nullptr;
    EventPublisher::post(std::make_shared<SceneSpatialDeleted>(cmd->id()));
}
