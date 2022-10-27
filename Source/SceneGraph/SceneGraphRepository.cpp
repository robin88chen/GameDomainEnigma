#include "SceneGraphRepository.h"
#include "Camera.h"
#include "Frustum.h"
#include "Node.h"
#include "Pawn.h"
#include "Light.h"
#include "SceneGraphEvents.h"
#include "SceneGraphContracts.h"
#include "Frameworks/EventPublisher.h"
#include "GameEngine/ContractCommands.h"
#include "Frameworks/CommandBus.h"
#include "Platforms/PlatformLayerUtilities.h"
#include <cassert>

using namespace Enigma::SceneGraph;
using namespace Enigma::Frameworks;
using namespace Enigma::Engine;

DEFINE_RTTI(SceneGraph, SceneGraphRepository);

SceneGraphRepository::SceneGraphRepository(Frameworks::ServiceManager* srv_mngr) : ISystemService(srv_mngr)
{
    IMPLEMENT_RTTI(Enigma, SceneGraph, SceneGraphRepository, ISystemService);
    m_handSystem = GraphicCoordSys::LeftHand;
    m_needTick = false;

    m_spatialLinkageResolver = SpatialLinkageResolver([=](auto n) -> std::shared_ptr<Spatial> { return this->QuerySpatial(n); });

    CommandBus::Post(std::make_shared<RegisterContractFactory>(Node::TYPE_RTTI.GetName(), [=](auto c) { this->NodeContractFactory(c); }));
}

SceneGraphRepository::~SceneGraphRepository()
{
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

void SceneGraphRepository::NodeContractFactory(const Contract& contract)
{
    if (contract.GetRtti().GetRttiName() != Node::TYPE_RTTI.GetName())
    {
        Platforms::Debug::ErrorPrintf("wrong contract rtti %s for node factory", contract.GetRtti().GetRttiName().c_str());
        return;
    }
    NodeContract node_contract = NodeContract::FromContract(contract);
    assert(!HasNode(node_contract.Name()));
    auto node = std::make_shared<Node>(node_contract, m_spatialLinkageResolver);
    std::lock_guard locker{ m_nodeMapLock };
    m_nodes.insert_or_assign(node_contract.Name(), node);
    EventPublisher::Post(std::make_shared<ContractedSpatialCreated>(node_contract.Name(), node));
}
