#include "SceneGraphRepository.h"
#include "Camera.h"
#include "Frustum.h"
#include "Node.h"
#include "Pawn.h"
#include "Light.h"
#include "LazyNode.h"
#include "VisibilityManagedNode.h"
#include "SceneGraphEvents.h"
#include "SceneGraphDtos.h"
#include "SceneGraphCommands.h"
#include "SceneGraphErrors.h"
#include "PortalZoneNode.h"
#include "Portal.h"
#include "PortalManagementNode.h"
#include "Frameworks/EventPublisher.h"
#include "Frameworks/CommandBus.h"
#include "SceneGraphBuilder.h"
#include "Platforms/PlatformLayerUtilities.h"
#include "Platforms/MemoryAllocMacro.h"
#include "Renderer/ModelPrimitive.h"
#include "Platforms/MemoryMacro.h"
#include "CameraFrustumCommands.h"
#include "CameraFrustumEvents.h"
#include <cassert>

using namespace Enigma::SceneGraph;
using namespace Enigma::Frameworks;
using namespace Enigma::Engine;
using namespace Enigma::Platforms;
using namespace Enigma::Renderer;

DEFINE_RTTI(SceneGraph, SceneGraphRepository, ISystemService);

SceneGraphRepository::SceneGraphRepository(Frameworks::ServiceManager* srv_mngr,
    const std::shared_ptr<Engine::IDtoDeserializer>& dto_deserializer) : ISystemService(srv_mngr)
{
    m_handSystem = GraphicCoordSys::LeftHand;

    m_needTick = false;
    m_builder = menew SceneGraphBuilder(this, dto_deserializer);
}

SceneGraphRepository::~SceneGraphRepository()
{
    SAFE_DELETE(m_builder);
}

ServiceResult SceneGraphRepository::onInit()
{
    m_doQueryingCamera = std::make_shared<CommandSubscriber>([=](auto c) { DoQueryingCamera(c); });
    CommandBus::Subscribe(typeid(SceneGraph::QueryCamera), m_doQueryingCamera);
    m_doCreatingCamera = std::make_shared<CommandSubscriber>([=](auto c) { DoCreatingCamera(c); });
    CommandBus::Subscribe(typeid(SceneGraph::CreateCamera), m_doCreatingCamera);

    return ServiceResult::Complete;
}
ServiceResult SceneGraphRepository::onTerm()
{
    CommandBus::Unsubscribe(typeid(SceneGraph::QueryCamera), m_doQueryingCamera);
    m_doQueryingCamera = nullptr;
    CommandBus::Unsubscribe(typeid(SceneGraph::CreateCamera), m_doCreatingCamera);
    m_doCreatingCamera = nullptr;

    return ServiceResult::Complete;
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

std::shared_ptr<Camera> SceneGraphRepository::CreateCamera(const GenericDto& dto)
{
    assert(!HasCamera(dto.getName()));
    auto camera = std::make_shared<Camera>(dto);
    std::lock_guard locker{ m_cameraMapLock };
    m_cameras.insert_or_assign(dto.getName(), camera);
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

std::shared_ptr<Node> SceneGraphRepository::CreateNode(const std::string& name, const Rtti& rtti)
{
    assert(!HasNode(name));
    std::shared_ptr<Node> node = nullptr;
    if (rtti == Node::TYPE_RTTI)
    {
        node = std::make_shared<Node>(name);
    }
    else if (rtti == LazyNode::TYPE_RTTI)
    {
        node = std::make_shared<LazyNode>(name);
    }
    else if (rtti == VisibilityManagedNode::TYPE_RTTI)
    {
        node = std::make_shared<VisibilityManagedNode>(name);
    }
    else if (rtti == PortalZoneNode::TYPE_RTTI)
    {
        node = std::make_shared<PortalZoneNode>(name);
    }
    else if (rtti == PortalManagementNode::TYPE_RTTI)
    {
        node = std::make_shared<PortalManagementNode>(name);
    }
    assert(node);
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

std::shared_ptr<Portal> SceneGraphRepository::CreatePortal(const std::string& name)
{
    assert(!HasPortal(name));
    auto portal = std::make_shared<Portal>(name);
    std::lock_guard locker{ m_portalMapLock };
    m_portals.insert_or_assign(name, portal);
    return portal;
}

bool SceneGraphRepository::HasPortal(const std::string& name)
{
    std::lock_guard locker{ m_portalMapLock };
    auto it = m_portals.find(name);
    return ((it != m_portals.end()) && (!it->second.expired()));
}

std::shared_ptr<Portal> SceneGraphRepository::QueryPortal(const std::string& name)
{
    std::lock_guard locker{ m_portalMapLock };
    auto it = m_portals.find(name);
    if (it == m_portals.end()) return nullptr;
    if (it->second.expired()) return nullptr;
    return it->second.lock();
}

std::shared_ptr<Spatial> SceneGraphRepository::QuerySpatial(const std::string& name)
{
    if (auto node = QueryNode(name)) return node;
    if (auto pawn = QueryPawn(name)) return pawn;
    if (auto light = QueryLight(name)) return light;
    if (auto portal = QueryPortal(name)) return portal;
    return nullptr;
}

std::shared_ptr<Spatial> SceneGraphRepository::AddNewSpatial(Spatial* spatial)
{
    assert(spatial);
    if (auto pawn = std::shared_ptr<Pawn>(dynamic_cast<Pawn*>(spatial)))
    {
        assert(!HasPawn(pawn->GetSpatialName()));
        std::lock_guard locker{ m_pawnMapLock };
        m_pawns.insert_or_assign(pawn->GetSpatialName(), pawn);
        return pawn;
    }
    else if (auto node = std::shared_ptr<Node>(dynamic_cast<Node*>(spatial)))
    {
        assert(!HasNode(node->GetSpatialName()));
        std::lock_guard locker{ m_nodeMapLock };
        m_nodes.insert_or_assign(node->GetSpatialName(), node);
        return node;
    }
    else if (auto portal = std::shared_ptr<Portal>(dynamic_cast<Portal*>(spatial)))
    {
        assert(!HasPortal(portal->GetSpatialName()));
        std::lock_guard locker{ m_portalMapLock };
        m_portals.insert_or_assign(portal->GetSpatialName(), portal);
        return portal;
    }
    else if (auto light = std::shared_ptr<Light>(dynamic_cast<Light*>(spatial)))
    {
        assert(!HasLight(light->GetSpatialName()));
        std::lock_guard locker{ m_lightMapLock };
        m_lights.insert_or_assign(light->GetSpatialName(), light);
        Frameworks::EventPublisher::Post(std::make_shared<LightInfoCreated>(light));
        return light;
    }
    else
    {
        assert(false);
        return nullptr;
    }
}

void SceneGraphRepository::DoQueryingCamera(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    if (const auto cmd = std::dynamic_pointer_cast<SceneGraph::QueryCamera>(c))
    {
        if (auto camera = QueryCamera(cmd->getName()))
        {
            EventPublisher::Post(std::make_shared<SceneGraph::ReplyCameraQuery>(cmd->getRuid(), camera));
        }
        else
        {
            EventPublisher::Post(std::make_shared<SceneGraph::QueryCameraFailed>(cmd->getRuid(), ErrorCode::cameraNotFound));
        }
    }
    else
    {
        assert(false);
    }
}

void SceneGraphRepository::DoCreatingCamera(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    if (const auto cmd = std::dynamic_pointer_cast<SceneGraph::CreateCamera>(c))
    {
        std::shared_ptr<Camera> camera = nullptr;
        if (HasCamera(cmd->GetDto().getName()))
        {
            camera = QueryCamera(cmd->GetDto().getName());
        }
        else
        {
            camera = CreateCamera(cmd->GetDto());
        }
        if (camera)
        {
            EventPublisher::Post(std::make_shared<SceneGraph::CameraCreated>(camera->getName(), camera));
        }
        else
        {
            EventPublisher::Post(std::make_shared<SceneGraph::CreateCameraFailed>(cmd->GetDto().getName(), ErrorCode::sceneRepositoryFailed));
        }
    }
    else
    {
        assert(false);
    }
}
