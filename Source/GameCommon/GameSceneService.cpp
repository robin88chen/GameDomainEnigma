#include "GameSceneService.h"
#include "SceneGraph/SceneGraphRepository.h"
#include "SceneGraph/Node.h"
#include "SceneGraph/PortalManagementNode.h"
#include "SceneGraph/FindSpatialByName.h"
#include "Platforms/MemoryMacro.h"
#include "Frameworks/EventPublisher.h"
#include "GameCameraEvents.h"

using namespace Enigma::GameCommon;
using namespace Enigma::Frameworks;
using namespace Enigma::SceneGraph;
using namespace Enigma::MathLib;

DEFINE_RTTI(GameCommon, GameSceneService, ISystemService);

#define SCENE_ROOT_NAME "_SceneRoot_"
#define PORTAL_MGT_NODE_NAME "_PortalManagementNode_"

GameSceneService::GameSceneService(ServiceManager* mngr, const std::shared_ptr<SceneGraphRepository>& scene_graph_repository) : ISystemService(mngr)
{
    m_sceneGraphRepository = scene_graph_repository;
    m_needTick = true;
    m_culler = nullptr;
}

GameSceneService::~GameSceneService()
{
    assert(!m_culler);
}

ServiceResult GameSceneService::OnInit()
{
    m_onCameraCreated = std::make_shared<EventSubscriber>([=](auto e) { OnGameCameraCreated(e); });
    m_onCameraUpdated = std::make_shared<EventSubscriber>([=](auto e) { OnGameCameraUpdated(e); });
    EventPublisher::Subscribe(typeid(GameCameraCreated), m_onCameraCreated);
    EventPublisher::Subscribe(typeid(GameCameraUpdated), m_onCameraUpdated);

    return ServiceResult::Complete;
}

ServiceResult GameSceneService::OnTick()
{
    if (m_culler)
    {
        m_culler->ComputeVisibleSet(GetSceneRoot());
    }

    return ServiceResult::Pendding;
}

ServiceResult GameSceneService::OnTerm()
{
    EventPublisher::Unsubscribe(typeid(GameCameraCreated), m_onCameraCreated);
    EventPublisher::Unsubscribe(typeid(GameCameraUpdated), m_onCameraUpdated);
    m_onCameraCreated = nullptr;
    m_onCameraUpdated = nullptr;

    DestroyRootScene();
    DestroySceneCuller();

    return ServiceResult::Complete;
}

void GameSceneService::CreateRootScene()
{
    assert(!m_sceneGraphRepository.expired());
    m_sceneRoot = m_sceneGraphRepository.lock()->CreateNode(SCENE_ROOT_NAME, Node::TYPE_RTTI.GetName());
    m_sceneRoot->SetLocalTransform(Matrix4::IDENTITY);

    m_portalMgtNode = std::dynamic_pointer_cast<PortalManagementNode, Node>(
        m_sceneGraphRepository.lock()->CreateNode(PORTAL_MGT_NODE_NAME, PortalManagementNode::TYPE_RTTI.GetName()));
    m_sceneRoot->AttachChild(m_portalMgtNode, Matrix4::IDENTITY);
}

void GameSceneService::DestroyRootScene()
{
    m_sceneRoot = nullptr;
    m_portalMgtNode = nullptr;
}

std::shared_ptr<Spatial> GameSceneService::FindSpatialByName(const std::string& spatial_name)
{
    if (!GetSceneRoot()) return nullptr;

    SceneGraph::FindSpatialByName finder(spatial_name);
    SceneTraveler::TravelResult res = GetSceneRoot()->VisitBy(&finder);
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
