#include "SceneGraphRepository.h"
#include "Camera.h"
#include "Frustum.h"
#include "Node.h"
#include "Pawn.h"
#include "Light.h"
#include "SceneGraphEvents.h"
#include "SceneGraphContracts.h"
#include "SceneGraphCommands.h"
#include "Frameworks/EventPublisher.h"
#include "GameEngine/ContractCommands.h"
#include "Frameworks/CommandBus.h"
#include "SceneGraphBuilder.h"
#include "Platforms/PlatformLayerUtilities.h"
#include "Platforms/MemoryAllocMacro.h"
#include <cassert>

using namespace Enigma::SceneGraph;
using namespace Enigma::Frameworks;
using namespace Enigma::Engine;
using namespace Enigma::Platforms;

DEFINE_RTTI(SceneGraph, SceneGraphRepository);

SceneGraphRepository::SceneGraphRepository(Frameworks::ServiceManager* srv_mngr) : ISystemService(srv_mngr)
{
    IMPLEMENT_RTTI(Enigma, SceneGraph, SceneGraphRepository, ISystemService);
    m_handSystem = GraphicCoordSys::LeftHand;
    m_needTick = false;
    m_builder = menew SceneGraphBuilder(this);
    m_doBuildingSceneGraph =
        std::make_shared<CommandSubscriber>([=](auto c) { this->DoBuildingSceneGraph(c); });
    CommandBus::Subscribe(typeid(SceneGraph::BuildSceneGraph), m_doBuildingSceneGraph);
}

SceneGraphRepository::~SceneGraphRepository()
{
    CommandBus::Unsubscribe(typeid(SceneGraph::BuildSceneGraph), m_doBuildingSceneGraph);
    m_doBuildingSceneGraph = nullptr;

    delete m_builder;
    CommandBus::Post(std::make_shared<UnRegisterContractFactory>(Node::TYPE_RTTI.GetName()));
}

void SceneGraphRepository::SetCoordinateSystem(GraphicCoordSys hand)
{
    m_handSystem = hand;
}

GraphicCoordSys SceneGraphRepository::GetCoordinateSystem()
{
    return m_handSystem;
}

std::shared_ptr<Camera> SceneGraphRepository::CreateCamera(const std::string& name)
{
    assert(!HasCamera(name));
    auto camera = std::make_shared<Camera>(name, m_handSystem);
    std::lock_guard locker{ m_cameraMapLock };
    m_cameras.insert_or_assign(name, camera);
    return camera;
}

bool SceneGraphRepository::HasCamera(const std::string& name)
{
    std::lock_guard locker{ m_cameraMapLock };
    auto it = m_cameras.find(name);
    return ((it != m_cameras.end()) && (!it->second.expired()));
}

std::shared_ptr<Camera> SceneGraphRepository::QueryCamera(const std::string& name)
{
    std::lock_guard locker{ m_cameraMapLock };
    auto it = m_cameras.find(name);
    if (it == m_cameras.end()) return nullptr;
    if (it->second.expired()) return nullptr;
    return it->second.lock();
}

std::shared_ptr<Frustum> SceneGraphRepository::CreateFrustum(const std::string& name, Frustum::ProjectionType proj)
{
    assert(!HasFrustum(name));
    auto frustum = std::make_shared<Frustum>(name, m_handSystem, proj);
    std::lock_guard locker{ m_frustumMapLock };
    m_frustums.insert_or_assign(name, frustum);
    return frustum;
}

bool SceneGraphRepository::HasFrustum(const std::string& name)
{
    std::lock_guard locker{ m_frustumMapLock };
    auto it = m_frustums.find(name);
    return ((it != m_frustums.end()) && (!it->second.expired()));
}

std::shared_ptr<Frustum> SceneGraphRepository::QueryFrustum(const std::string& name)
{
    std::lock_guard locker{ m_frustumMapLock };
    auto it = m_frustums.find(name);
    if (it == m_frustums.end()) return nullptr;
    if (it->second.expired()) return nullptr;
    return it->second.lock();
}

std::shared_ptr<Node> SceneGraphRepository::CreateNode(const std::string& name)
{
    assert(!HasNode(name));
    auto node = std::make_shared<Node>(name);
    std::lock_guard locker{ m_nodeMapLock };
    m_nodes.insert_or_assign(name, node);
    return node;
}

std::shared_ptr<Node> SceneGraphRepository::CreateNode(const NodeContract& contract)
{
    assert(!HasNode(contract.Name()));
    auto node = std::make_shared<Node>(contract);
    std::lock_guard locker{ m_nodeMapLock };
    m_nodes.insert_or_assign(contract.Name(), node);
    return node;
}

bool SceneGraphRepository::HasNode(const std::string& name)
{
    std::lock_guard locker{ m_nodeMapLock };
    auto it = m_nodes.find(name);
    return ((it != m_nodes.end()) && (!it->second.expired()));
}

std::shared_ptr<Node> SceneGraphRepository::QueryNode(const std::string& name)
{
    std::lock_guard locker{ m_nodeMapLock };
    auto it = m_nodes.find(name);
    if (it == m_nodes.end()) return nullptr;
    if (it->second.expired()) return nullptr;
    return it->second.lock();
}

std::shared_ptr<Pawn> SceneGraphRepository::CreatePawn(const std::string& name)
{
    assert(!HasPawn(name));
    auto pawn = std::make_shared<Pawn>(name);
    std::lock_guard locker{ m_pawnMapLock };
    m_pawns.insert_or_assign(name, pawn);
    return pawn;
}

bool SceneGraphRepository::HasPawn(const std::string& name)
{
    std::lock_guard locker{ m_pawnMapLock };
    auto it = m_pawns.find(name);
    return ((it != m_pawns.end()) && (!it->second.expired()));
}

std::shared_ptr<Pawn> SceneGraphRepository::QueryPawn(const std::string& name)
{
    std::lock_guard locker{ m_pawnMapLock };
    auto it = m_pawns.find(name);
    if (it == m_pawns.end()) return nullptr;
    if (it->second.expired()) return nullptr;
    return it->second.lock();
}

std::shared_ptr<Light> SceneGraphRepository::CreateLight(const std::string& name, const LightInfo& info)
{
    assert(!HasLight(name));
    auto light = std::make_shared<Light>(name, info);
    std::lock_guard locker{ m_lightMapLock };
    m_lights.insert_or_assign(name, light);
    Frameworks::EventPublisher::Post(std::make_shared<LightInfoCreated>(light));
    return light;
}

std::shared_ptr<Light> SceneGraphRepository::CreateLight(const LightContract& contract)
{
    assert(!HasLight(contract.Name()));
    auto light = std::make_shared<Light>(contract);
    std::lock_guard locker{ m_lightMapLock };
    m_lights.insert_or_assign(contract.Name(), light);
    Frameworks::EventPublisher::Post(std::make_shared<LightInfoCreated>(light));
    return light;
}

bool SceneGraphRepository::HasLight(const std::string& name)
{
    std::lock_guard locker{ m_lightMapLock };
    auto it = m_lights.find(name);
    return ((it != m_lights.end()) && (!it->second.expired()));
}

std::shared_ptr<Light> SceneGraphRepository::QueryLight(const std::string& name)
{
    std::lock_guard locker{ m_lightMapLock };
    auto it = m_lights.find(name);
    if (it == m_lights.end()) return nullptr;
    if (it->second.expired()) return nullptr;
    return it->second.lock();
}

std::shared_ptr<Spatial> SceneGraphRepository::QuerySpatial(const std::string& name)
{
    if (auto node = QueryNode(name)) return node;
    if (auto pawn = QueryPawn(name)) return pawn;
    if (auto light = QueryLight(name)) return light;
    return nullptr;
}

void SceneGraphRepository::DoBuildingSceneGraph(const ICommandPtr& c)
{
    if (!m_builder) return;
    if (!c) return;
    auto cmd = std::dynamic_pointer_cast<SceneGraph::BuildSceneGraph, ICommand>(c);
    if (!cmd) return;
    m_builder->BuildSceneGraph(cmd->GetSceneGraphId(), cmd->GetContracts());
}
