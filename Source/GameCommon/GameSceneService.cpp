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
#include "SceneGraph/NodalSceneGraph.h"
#include "SceneGraph/PortalSceneGraph.h"
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

    m_createNodalSceneRoot = std::make_shared<CommandSubscriber>([=](auto c) { createSceneRoot(c); });
    CommandBus::subscribe(typeid(CreateNodalSceneRoot), m_createNodalSceneRoot);
    m_createPortalSceneRoot = std::make_shared<CommandSubscriber>([=](auto c) { createSceneRoot(c); });
    CommandBus::subscribe(typeid(CreatePortalSceneRoot), m_createPortalSceneRoot);
    m_attachSceneRootChild = std::make_shared<CommandSubscriber>([=](auto c) { attachSceneRootChild(c); });
    CommandBus::subscribe(typeid(AttachSceneRootChild), m_attachSceneRootChild);

    return ServiceResult::Complete;
}

ServiceResult GameSceneService::onTick()
{
    if (m_culler)
    {
        m_culler->ComputeVisibleSet(m_sceneGraph->root());
    }

    return ServiceResult::Pendding;
}

ServiceResult GameSceneService::onTerm()
{
    if (m_sceneGraph) m_sceneGraph->unregisterHandlers();

    EventPublisher::unsubscribe(typeid(GameCameraCreated), m_onCameraCreated);
    m_onCameraCreated = nullptr;
    EventPublisher::unsubscribe(typeid(GameCameraUpdated), m_onCameraUpdated);
    m_onCameraUpdated = nullptr;
    CommandBus::unsubscribe(typeid(CreateNodalSceneRoot), m_createNodalSceneRoot);
    m_createNodalSceneRoot = nullptr;
    CommandBus::unsubscribe(typeid(CreatePortalSceneRoot), m_createPortalSceneRoot);
    m_createPortalSceneRoot = nullptr;
    CommandBus::unsubscribe(typeid(AttachSceneRootChild), m_attachSceneRootChild);
    m_attachSceneRootChild = nullptr;

    destroyRootScene();
    destroySceneCuller();

    return ServiceResult::Complete;
}

void GameSceneService::createNodalSceneRoot(const SceneGraph::SpatialId& scene_root_id)
{
    assert(!m_sceneGraphRepository.expired());
    assert(!scene_root_id.empty());
    if (m_sceneGraph == nullptr)
    {
        m_sceneGraph = std::make_unique<NodalSceneGraph>(m_sceneGraphRepository.lock());
        m_sceneGraph->registerHandlers();
    }
    else
    {
        auto& r = *m_sceneGraph.get();
        assert(typeid(r) == typeid(NodalSceneGraph));
    }
    const auto er = m_sceneGraph->createRoot(scene_root_id);
    if (er)
    {
        EventPublisher::post(std::make_shared<CreateNodalSceneRootFailed>(scene_root_id, er));
        return;
    }
    if ((!m_cameraService.expired()) && m_cameraService.lock()->primaryCamera())
    {
        createSceneCuller(m_cameraService.lock()->primaryCamera());
    }
    EventPublisher::post(std::make_shared<NodalSceneRootCreated>(m_sceneGraph->root()));
}

void GameSceneService::createPortalSceneRoot(const SceneGraph::SpatialId& scene_root_id)
{
    assert(!m_sceneGraphRepository.expired());
    assert(!scene_root_id.empty());
    if (m_sceneGraph == nullptr)
    {
        m_sceneGraph = std::make_unique<PortalSceneGraph>(m_sceneGraphRepository.lock());
        m_sceneGraph->registerHandlers();
    }
    else
    {
        auto& r = *m_sceneGraph.get();
        assert(typeid(r) == typeid(PortalSceneGraph));
    }
    const auto er = m_sceneGraph->createRoot(scene_root_id);
    if (er)
    {
        EventPublisher::post(std::make_shared<CreatePortalSceneRootFailed>(scene_root_id, er));
        return;
    }
    if ((!m_cameraService.expired()) && m_cameraService.lock()->primaryCamera())
    {
        createSceneCuller(m_cameraService.lock()->primaryCamera());
    }
    EventPublisher::post(std::make_shared<PortalSceneRootCreated>(m_sceneGraph->root()));
}

void GameSceneService::destroyRootScene()
{
    if (m_sceneGraph) m_sceneGraph->destroyRoot();
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

void GameSceneService::createSceneRoot(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    if (const auto cmd = std::dynamic_pointer_cast<CreateNodalSceneRoot, ICommand>(c))
    {
        if (cmd->sceneRootId().empty())
        {
            EventPublisher::post(std::make_shared<CreateNodalSceneRootFailed>(cmd->sceneRootId(), ErrorCode::invalidSceneRootId));
            return;
        }
        if ((m_sceneGraph) && (m_sceneGraph->root()))
        {
            EventPublisher::post(std::make_shared<CreateNodalSceneRootFailed>(cmd->sceneRootId(), ErrorCode::sceneRootAlreadyExist));
            return;
        }
        createNodalSceneRoot(cmd->sceneRootId());
    }
    else if (const auto cmd = std::dynamic_pointer_cast<CreatePortalSceneRoot, ICommand>(c))
    {
        if (cmd->portalManagementNodeId().empty())
        {
            EventPublisher::post(std::make_shared<CreatePortalSceneRootFailed>(cmd->portalManagementNodeId(), ErrorCode::invalidSceneRootId));
            return;
        }
        if ((m_sceneGraph) && (m_sceneGraph->root()))
        {
            EventPublisher::post(std::make_shared<CreatePortalSceneRootFailed>(cmd->portalManagementNodeId(), ErrorCode::sceneRootAlreadyExist));
            return;
        }
        createPortalSceneRoot(cmd->portalManagementNodeId());
    }
}

void GameSceneService::attachSceneRootChild(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<AttachSceneRootChild, ICommand>(c);
    if (!cmd) return;
    if (!cmd->child()) return;
    if (!m_sceneGraph->root())
    {
        EventPublisher::post(std::make_shared<AttachSceneRootChildFailed>(cmd->child()->id(), ErrorCode::nullSceneRoot));
        return;
    }
    if (error er = m_sceneGraph->root()->attachChild(cmd->child(), cmd->localTransform()))
    {
        EventPublisher::post(std::make_shared<AttachSceneRootChildFailed>(cmd->child()->id(), er));
    }
    else
    {
        EventPublisher::post(std::make_shared<SceneRootChildAttached>(cmd->child()));
    }
}
