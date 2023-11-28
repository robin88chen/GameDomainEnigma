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
#include "Frameworks/QueryDispatcher.h"
#include "SceneGraphBuilder.h"
#include "Platforms/PlatformLayerUtilities.h"
#include "Platforms/MemoryAllocMacro.h"
#include "Renderer/ModelPrimitive.h"
#include "Platforms/MemoryMacro.h"
#include "CameraFrustumCommands.h"
#include "CameraFrustumEvents.h"
#include "SceneGraphQueries.h"
#include "PortalDtos.h"
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
    m_queryCamera = std::make_shared<QuerySubscriber>([=](const IQueryPtr& q) { queryCamera(q); });
    QueryDispatcher::subscribe(typeid(QueryCamera), m_queryCamera);
    m_queryNode = std::make_shared<QuerySubscriber>([=](const IQueryPtr& q) { queryNode(q); });
    QueryDispatcher::subscribe(typeid(QueryNode), m_queryNode);
    m_createCamera = std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { createCamera(c); });
    CommandBus::subscribe(typeid(CreateCamera), m_createCamera);
    m_createNode = std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { createNode(c); });
    CommandBus::subscribe(typeid(CreateNode), m_createNode);

    return ServiceResult::Complete;
}
ServiceResult SceneGraphRepository::onTerm()
{
    QueryDispatcher::unsubscribe(typeid(QueryCamera), m_queryCamera);
    m_queryCamera = nullptr;
    QueryDispatcher::unsubscribe(typeid(QueryNode), m_queryNode);
    m_queryNode = nullptr;
    CommandBus::unsubscribe(typeid(CreateCamera), m_createCamera);
    m_createCamera = nullptr;
    CommandBus::unsubscribe(typeid(CreateNode), m_createNode);
    m_createNode = nullptr;

    return ServiceResult::Complete;
}

void SceneGraphRepository::setCoordinateSystem(GraphicCoordSys hand)
{
    m_handSystem = hand;
}

GraphicCoordSys SceneGraphRepository::getCoordinateSystem()
{
    return m_handSystem;
}

std::shared_ptr<Camera> SceneGraphRepository::createCamera(const std::string& name)
{
    assert(!hasCamera(name));
    auto camera = std::make_shared<Camera>(name, m_handSystem);
    std::lock_guard locker{ m_cameraMapLock };
    m_cameras.insert_or_assign(name, camera);
    return camera;
}

std::shared_ptr<Camera> SceneGraphRepository::createCamera(const GenericDto& dto)
{
    assert(!hasCamera(dto.getName()));
    auto camera = std::make_shared<Camera>(dto);
    std::lock_guard locker{ m_cameraMapLock };
    m_cameras.insert_or_assign(dto.getName(), camera);
    return camera;
}

bool SceneGraphRepository::hasCamera(const std::string& name)
{
    std::lock_guard locker{ m_cameraMapLock };
    auto it = m_cameras.find(name);
    return ((it != m_cameras.end()) && (!it->second.expired()));
}

std::shared_ptr<Camera> SceneGraphRepository::queryCamera(const std::string& name)
{
    std::lock_guard locker{ m_cameraMapLock };
    auto it = m_cameras.find(name);
    if (it == m_cameras.end()) return nullptr;
    if (it->second.expired()) return nullptr;
    return it->second.lock();
}

std::shared_ptr<Node> SceneGraphRepository::createNode(const std::string& name, const Engine::FactoryDesc& factory_desc)
{
    assert(!hasNode(name));
    std::shared_ptr<Node> node = nullptr;
    if (factory_desc.GetRttiName() == Node::TYPE_RTTI.getName())
    {
        node = std::make_shared<Node>(name);
    }
    else if (factory_desc.GetRttiName() == LazyNode::TYPE_RTTI.getName())
    {
        node = std::make_shared<LazyNode>(name, factory_desc);
    }
    else if (factory_desc.GetRttiName() == VisibilityManagedNode::TYPE_RTTI.getName())
    {
        node = std::make_shared<VisibilityManagedNode>(name, factory_desc);
    }
    else if (factory_desc.GetRttiName() == PortalZoneNode::TYPE_RTTI.getName())
    {
        node = std::make_shared<PortalZoneNode>(name, factory_desc);
    }
    else if (factory_desc.GetRttiName() == PortalManagementNode::TYPE_RTTI.getName())
    {
        node = std::make_shared<PortalManagementNode>(name);
    }
    assert(node);
    std::lock_guard locker{ m_nodeMapLock };
    m_nodes.insert_or_assign(name, node);
    return node;
}

std::shared_ptr<Node> SceneGraphRepository::createNode(const GenericDto& dto)
{
    assert(!hasNode(dto.getName()));
    std::shared_ptr<Node> node = nullptr;
    if (dto.GetRtti().GetRttiName() == Node::TYPE_RTTI.getName())
    {
        node = std::make_shared<Node>(dto);
    }
    else if (dto.GetRtti().GetRttiName() == LazyNode::TYPE_RTTI.getName())
    {
        node = std::make_shared<LazyNode>(dto);
    }
    else if (dto.GetRtti().GetRttiName() == VisibilityManagedNode::TYPE_RTTI.getName())
    {
        node = createVisibilityManagedNode(VisibilityManagedNodeDto::fromGenericDto(dto));
    }
    else if (dto.GetRtti().GetRttiName() == PortalZoneNode::TYPE_RTTI.getName())
    {
        node = createPortalZoneNode(PortalZoneNodeDto::fromGenericDto(dto));
    }
    else if (dto.GetRtti().GetRttiName() == PortalManagementNode::TYPE_RTTI.getName())
    {
        node = std::make_shared<PortalManagementNode>(dto);
    }
    assert(node);
    std::lock_guard locker{ m_nodeMapLock };
    m_nodes.insert_or_assign(dto.getName(), node);
    return node;
}

std::shared_ptr<PortalZoneNode> SceneGraphRepository::createPortalZoneNode(const PortalZoneNodeDto& portal_zone_node_dto)
{
    auto node = std::make_shared<PortalZoneNode>(portal_zone_node_dto.toGenericDto());
    if (!portal_zone_node_dto.portalName().empty())
    {
        if (const auto portal = queryPortal(portal_zone_node_dto.portalName())) portal->setAdjacentZone(node);
    }
    else if (!portal_zone_node_dto.portalManagementNodeName().empty())
    {
        if (const auto portal_management = std::dynamic_pointer_cast<PortalManagementNode>(queryNode(portal_zone_node_dto.portalManagementNodeName())))
            portal_management->attachOutsideZone(node);
    }
    return node;
}

std::shared_ptr<VisibilityManagedNode> SceneGraphRepository::createVisibilityManagedNode(const VisibilityManagedNodeDto& visibility_managed_node_dto)
{
    auto node = std::make_shared<VisibilityManagedNode>(visibility_managed_node_dto.toGenericDto());
    if (!visibility_managed_node_dto.parentName().empty())
    {
        if (const auto parent = queryNode(visibility_managed_node_dto.parentName())) parent->attachChild(node, visibility_managed_node_dto.localTransform());
    }
    return node;
}

bool SceneGraphRepository::hasNode(const std::string& name)
{
    std::lock_guard locker{ m_nodeMapLock };
    auto it = m_nodes.find(name);
    return ((it != m_nodes.end()) && (!it->second.expired()));
}

std::shared_ptr<Node> SceneGraphRepository::queryNode(const std::string& name)
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
    Frameworks::EventPublisher::post(std::make_shared<LightInfoCreated>(light));
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

std::shared_ptr<Portal> SceneGraphRepository::createPortal(const std::string& name)
{
    assert(!hasPortal(name));
    auto portal = std::make_shared<Portal>(name);
    std::lock_guard locker{ m_portalMapLock };
    m_portals.insert_or_assign(name, portal);
    return portal;
}

bool SceneGraphRepository::hasPortal(const std::string& name)
{
    std::lock_guard locker{ m_portalMapLock };
    auto it = m_portals.find(name);
    return ((it != m_portals.end()) && (!it->second.expired()));
}

std::shared_ptr<Portal> SceneGraphRepository::queryPortal(const std::string& name)
{
    std::lock_guard locker{ m_portalMapLock };
    auto it = m_portals.find(name);
    if (it == m_portals.end()) return nullptr;
    if (it->second.expired()) return nullptr;
    return it->second.lock();
}

std::shared_ptr<Spatial> SceneGraphRepository::QuerySpatial(const std::string& name)
{
    if (auto node = queryNode(name)) return node;
    if (auto pawn = QueryPawn(name)) return pawn;
    if (auto light = QueryLight(name)) return light;
    if (auto portal = queryPortal(name)) return portal;
    return nullptr;
}

std::shared_ptr<Spatial> SceneGraphRepository::AddNewSpatial(Spatial* spatial)
{
    assert(spatial);
    if (auto pawn = std::shared_ptr<Pawn>(dynamic_cast<Pawn*>(spatial)))
    {
        assert(!HasPawn(pawn->getSpatialName()));
        std::lock_guard locker{ m_pawnMapLock };
        m_pawns.insert_or_assign(pawn->getSpatialName(), pawn);
        return pawn;
    }
    else if (auto node = std::shared_ptr<Node>(dynamic_cast<Node*>(spatial)))
    {
        assert(!hasNode(node->getSpatialName()));
        std::lock_guard locker{ m_nodeMapLock };
        m_nodes.insert_or_assign(node->getSpatialName(), node);
        return node;
    }
    else if (auto portal = std::shared_ptr<Portal>(dynamic_cast<Portal*>(spatial)))
    {
        assert(!hasPortal(portal->getSpatialName()));
        std::lock_guard locker{ m_portalMapLock };
        m_portals.insert_or_assign(portal->getSpatialName(), portal);
        return portal;
    }
    else if (auto light = std::shared_ptr<Light>(dynamic_cast<Light*>(spatial)))
    {
        assert(!HasLight(light->getSpatialName()));
        std::lock_guard locker{ m_lightMapLock };
        m_lights.insert_or_assign(light->getSpatialName(), light);
        Frameworks::EventPublisher::post(std::make_shared<LightInfoCreated>(light));
        return light;
    }
    else
    {
        assert(false);
        return nullptr;
    }
}

void SceneGraphRepository::queryCamera(const IQueryPtr& q)
{
    if (!q) return;
    const auto query = std::dynamic_pointer_cast<QueryCamera>(q);
    assert(query);
    query->setResult(queryCamera(query->cameraName()));
}

void SceneGraphRepository::queryNode(const Frameworks::IQueryPtr& q)
{
    if (!q) return;
    const auto query = std::dynamic_pointer_cast<QueryNode>(q);
    assert(query);
    query->setResult(queryNode(query->nodeName()));
}

void SceneGraphRepository::createCamera(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    if (const auto cmd = std::dynamic_pointer_cast<SceneGraph::CreateCamera>(c))
    {
        std::shared_ptr<Camera> camera = nullptr;
        if (hasCamera(cmd->GetDto().getName()))
        {
            camera = queryCamera(cmd->GetDto().getName());
        }
        else
        {
            camera = createCamera(cmd->GetDto());
        }
        if (camera)
        {
            EventPublisher::post(std::make_shared<SceneGraph::CameraCreated>(camera->getName(), camera));
        }
        else
        {
            EventPublisher::post(std::make_shared<SceneGraph::CreateCameraFailed>(cmd->GetDto().getName(), ErrorCode::sceneRepositoryFailed));
        }
    }
    else
    {
        assert(false);
    }
}

void SceneGraphRepository::createNode(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<CreateNode>(c);
    if (!cmd) return;
    if (hasNode(cmd->name()))
    {
        EventPublisher::post(std::make_shared<CreateNodeFailed>(cmd->getRuid(), ErrorCode::entityAlreadyExists));
    }
    else
    {
        auto node = createNode(cmd->name(), cmd->factoryDesc());
        if (node)
        {
            EventPublisher::post(std::make_shared<NodeCreated>(cmd->getRuid(), node));
        }
        else
        {
            EventPublisher::post(std::make_shared<CreateNodeFailed>(cmd->getRuid(), ErrorCode::sceneRepositoryFailed));
        }
    }
}
