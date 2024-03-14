#include "SceneGraphRepository.h"
#include "SceneGraphStoreMapper.h"
#include "SceneGraphFactory.h"
#include "SpatialId.h"
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
//#include "SceneGraphBuilder.h"
#include "Platforms/PlatformLayerUtilities.h"
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

DEFINE_RTTI(SceneGraph, SceneGraphRepository, ISystemService);

SceneGraphRepository::SceneGraphRepository(Frameworks::ServiceManager* srv_mngr, const std::shared_ptr<SceneGraphStoreMapper>& store_mapper) : ISystemService(srv_mngr)
{
    m_handSystem = GraphicCoordSys::LeftHand;
    m_storeMapper = store_mapper;
    m_factory = menew SceneGraphFactory();
    m_needTick = false;
    //m_builder = menew SceneGraphBuilder(this, dto_deserializer);
    registerHandlers();
}

SceneGraphRepository::~SceneGraphRepository()
{
    unregisterHandlers();
    //SAFE_DELETE(m_builder);
    SAFE_DELETE(m_factory);
}

ServiceResult SceneGraphRepository::onInit()
{
    m_storeMapper->connect();
    return ServiceResult::Complete;
}
ServiceResult SceneGraphRepository::onTerm()
{
    m_storeMapper->disconnect();

    m_cameras.clear();

    return ServiceResult::Complete;
}

void SceneGraphRepository::registerHandlers()
{
    m_queryCamera = std::make_shared<QuerySubscriber>([=](const IQueryPtr& q) { queryCamera(q); });
    QueryDispatcher::subscribe(typeid(QueryCamera), m_queryCamera);
    m_requestCameraCreation = std::make_shared<QuerySubscriber>([=](const IQueryPtr& q) { requestCameraCreation(q); });
    QueryDispatcher::subscribe(typeid(RequestCameraCreation), m_requestCameraCreation);
    m_requestCameraConstitution = std::make_shared<QuerySubscriber>([=](const IQueryPtr& q) { requestCameraConstitution(q); });
    QueryDispatcher::subscribe(typeid(RequestCameraConstitution), m_requestCameraConstitution);
    //m_queryNode = std::make_shared<QuerySubscriber>([=](const IQueryPtr& q) { queryNode(q); });
    //QueryDispatcher::subscribe(typeid(QueryNode), m_queryNode);
    m_querySpatial = std::make_shared<QuerySubscriber>([=](const IQueryPtr& q) { querySpatial(q); });
    QueryDispatcher::subscribe(typeid(QuerySpatial), m_querySpatial);
    m_requestSpatialCreation = std::make_shared<QuerySubscriber>([=](const IQueryPtr& q) { requestSpatialCreation(q); });
    QueryDispatcher::subscribe(typeid(RequestSpatialCreation), m_requestSpatialCreation);
    m_requestSpatialConstitution = std::make_shared<QuerySubscriber>([=](const IQueryPtr& q) { requestSpatialConstitution(q); });
    QueryDispatcher::subscribe(typeid(RequestSpatialConstitution), m_requestSpatialConstitution);
    m_requestLightCreation = std::make_shared<QuerySubscriber>([=](const IQueryPtr& q) { requestLightCreation(q); });
    QueryDispatcher::subscribe(typeid(RequestLightCreation), m_requestLightCreation);

    m_putCamera = std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { putCamera(c); });
    CommandBus::subscribe(typeid(PutCamera), m_putCamera);
    m_removeCamera = std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { removeCamera(c); });
    CommandBus::subscribe(typeid(RemoveCamera), m_removeCamera);
    m_putSpatial = std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { putSpatial(c); });
    CommandBus::subscribe(typeid(PutSpatial), m_putSpatial);
    m_removeSpatial = std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { removeSpatial(c); });
    CommandBus::subscribe(typeid(RemoveSpatial), m_removeSpatial);
    m_putLaziedContent = std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { putLaziedContent(c); });
    CommandBus::subscribe(typeid(PutLaziedContent), m_putLaziedContent);
    m_removeLaziedContent = std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { removeLaziedContent(c); });
    CommandBus::subscribe(typeid(RemoveLaziedContent), m_removeLaziedContent);

    m_attachNodeChild = std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { attachNodeChild(c); });
    CommandBus::subscribe(typeid(AttachNodeChild), m_attachNodeChild);
}

void SceneGraphRepository::unregisterHandlers()
{
    QueryDispatcher::unsubscribe(typeid(QueryCamera), m_queryCamera);
    m_queryCamera = nullptr;
    QueryDispatcher::unsubscribe(typeid(RequestCameraCreation), m_requestCameraCreation);
    m_requestCameraCreation = nullptr;
    QueryDispatcher::unsubscribe(typeid(RequestCameraConstitution), m_requestCameraConstitution);
    m_requestCameraConstitution = nullptr;
    //QueryDispatcher::unsubscribe(typeid(QueryNode), m_queryNode);
    //m_queryNode = nullptr;
    QueryDispatcher::unsubscribe(typeid(QuerySpatial), m_querySpatial);
    m_querySpatial = nullptr;
    QueryDispatcher::unsubscribe(typeid(RequestSpatialCreation), m_requestSpatialCreation);
    m_requestSpatialCreation = nullptr;
    QueryDispatcher::unsubscribe(typeid(RequestSpatialConstitution), m_requestCameraConstitution);
    m_requestSpatialConstitution = nullptr;
    QueryDispatcher::unsubscribe(typeid(RequestLightCreation), m_requestLightCreation);
    m_requestLightCreation = nullptr;

    CommandBus::unsubscribe(typeid(PutCamera), m_putCamera);
    m_putCamera = nullptr;
    CommandBus::unsubscribe(typeid(RemoveCamera), m_removeCamera);
    m_removeCamera = nullptr;
    CommandBus::unsubscribe(typeid(PutSpatial), m_putSpatial);
    m_putSpatial = nullptr;
    CommandBus::unsubscribe(typeid(RemoveSpatial), m_removeSpatial);
    m_removeSpatial = nullptr;
    CommandBus::unsubscribe(typeid(PutLaziedContent), m_putLaziedContent);
    m_putLaziedContent = nullptr;
    CommandBus::unsubscribe(typeid(RemoveLaziedContent), m_removeLaziedContent);
    m_removeLaziedContent = nullptr;

    CommandBus::unsubscribe(typeid(AttachNodeChild), m_attachNodeChild);
    m_attachNodeChild = nullptr;
}

void SceneGraphRepository::setCoordinateSystem(GraphicCoordSys hand)
{
    m_handSystem = hand;
}

GraphicCoordSys SceneGraphRepository::getCoordinateSystem()
{
    return m_handSystem;
}

bool SceneGraphRepository::hasCamera(const SpatialId& id)
{
    assert(m_storeMapper);
    std::lock_guard locker{ m_cameraMapLock };
    auto it = m_cameras.find(id);
    if (it != m_cameras.end()) return true;
    return m_storeMapper->hasCamera(id);
}

std::shared_ptr<Camera> SceneGraphRepository::queryCamera(const SpatialId& id)
{
    if (!hasCamera(id)) return nullptr;
    std::lock_guard locker{ m_cameraMapLock };
    auto it = m_cameras.find(id);
    if (it != m_cameras.end()) return it->second;
    auto dto = m_storeMapper->queryCamera(id);
    assert(dto.has_value());
    auto camera = m_factory->constituteCamera(id, dto.value(), true);
    assert(camera);
    m_cameras.insert_or_assign(id, camera);
    return camera;
}

/*std::shared_ptr<Node> SceneGraphRepository::createNode(const SpatialId& id)
{
    return createNode(id.name(), id.rtti().getName());
}
*/
/*std::shared_ptr<Node> SceneGraphRepository::createNode(const std::string& name, const Engine::FactoryDesc& factory_desc)
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
}*/

/*std::shared_ptr<Node> SceneGraphRepository::createNode(const GenericDto& dto)
{
    assert(!hasNode(dto.getName()));
    std::shared_ptr<Node> node = nullptr;
    if (dto.getRtti().GetRttiName() == Node::TYPE_RTTI.getName())
    {
        node = std::make_shared<Node>(dto);
    }
    else if (dto.getRtti().GetRttiName() == LazyNode::TYPE_RTTI.getName())
    {
        node = std::make_shared<LazyNode>(dto);
    }
    else if (dto.getRtti().GetRttiName() == VisibilityManagedNode::TYPE_RTTI.getName())
    {
        node = createVisibilityManagedNode(VisibilityManagedNodeDto{ dto });
    }
    else if (dto.getRtti().GetRttiName() == PortalZoneNode::TYPE_RTTI.getName())
    {
        node = createPortalZoneNode(PortalZoneNodeDto::fromGenericDto(dto));
    }
    else if (dto.getRtti().GetRttiName() == PortalManagementNode::TYPE_RTTI.getName())
    {
        node = std::make_shared<PortalManagementNode>(dto);
    }
    assert(node);
    std::lock_guard locker{ m_nodeMapLock };
    m_nodes.insert_or_assign(dto.getName(), node);
    return node;
}*/

/*std::shared_ptr<PortalZoneNode> SceneGraphRepository::createPortalZoneNode(const PortalZoneNodeDto& portal_zone_node_dto)
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
}*/

/*std::shared_ptr<VisibilityManagedNode> SceneGraphRepository::createVisibilityManagedNode(const VisibilityManagedNodeDto& visibility_managed_node_dto)
{
    auto node = std::make_shared<VisibilityManagedNode>(visibility_managed_node_dto.toGenericDto());
    if (!visibility_managed_node_dto.parentName().empty())
    {
        if (const auto parent = queryNode(visibility_managed_node_dto.parentName())) parent->attachChild(node, visibility_managed_node_dto.localTransform());
    }
    return node;
}*/

/*bool SceneGraphRepository::hasNode(const std::string& name)
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
}*/

bool SceneGraphRepository::hasSpatial(const SpatialId& id)
{
    assert(m_storeMapper);
    assert(id.rtti().isDerived(Spatial::TYPE_RTTI));
    std::lock_guard locker{ m_spatialMapLock };
    auto it = m_spatials.find(id);
    if (it != m_spatials.end()) return true;
    return m_storeMapper->hasSpatial(id);
}

std::shared_ptr<Spatial> SceneGraphRepository::querySpatial(const SpatialId& id)
{
    if (!hasSpatial(id)) return nullptr;
    std::lock_guard locker{ m_spatialMapLock };
    auto it = m_spatials.find(id);
    if (it != m_spatials.end()) return it->second;
    auto dto = m_storeMapper->querySpatial(id);
    assert(dto.has_value());
    return m_factory->constituteSpatial(id, dto.value(), true);
}

bool SceneGraphRepository::hasLaziedContent(const SpatialId& id)
{
    assert(m_storeMapper);
    assert(id.rtti().isDerived(LazyNode::TYPE_RTTI));
    return m_storeMapper->hasLaziedContent(id);
}

/*std::shared_ptr<Portal> SceneGraphRepository::createPortal(const std::string& name)
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
}*/

/*std::shared_ptr<Spatial> SceneGraphRepository::querySpatial(const std::string& name)
{
    if (auto node = queryNode(name)) return node;
    //if (auto pawn = queryPawn(name)) return pawn;
    //if (auto light = QueryLight(name)) return light;
    //if (auto portal = queryPortal(name)) return portal;
    return nullptr;
}*/

/*std::shared_ptr<Spatial> SceneGraphRepository::AddNewSpatial(Spatial* spatial)
{
    assert(spatial);
    if (auto node = std::shared_ptr<Node>(dynamic_cast<Node*>(spatial)))
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
    else
    {
        assert(false);
        return nullptr;
    }
}*/

void SceneGraphRepository::queryCamera(const IQueryPtr& q)
{
    if (!q) return;
    const auto query = std::dynamic_pointer_cast<QueryCamera>(q);
    assert(query);
    query->setResult(queryCamera(query->id()));
}

void SceneGraphRepository::requestCameraCreation(const Frameworks::IQueryPtr& r)
{
    assert(m_factory);
    if (!r) return;
    auto request = std::dynamic_pointer_cast<RequestCameraCreation>(r);
    if (!request) return;
    if (hasCamera(request->id()))
    {
        EventPublisher::post(std::make_shared<CreateCameraFailed>(request->id(), ErrorCode::entityAlreadyExists));
        return;
    }
    auto camera = m_factory->createCamera(request->id());
    if (request->persistenceLevel() == PersistenceLevel::Repository)
    {
        std::lock_guard locker{ m_cameraMapLock };
        m_cameras.insert_or_assign(request->id(), camera);
    }
    else if (request->persistenceLevel() == PersistenceLevel::Store)
    {
        putCamera(camera);
    }
    request->setResult(camera);
}

void SceneGraphRepository::requestCameraConstitution(const Frameworks::IQueryPtr& r)
{
    assert(m_factory);
    if (!r) return;
    auto request = std::dynamic_pointer_cast<RequestCameraConstitution>(r);
    if (!request) return;
    if (hasCamera(request->id()))
    {
        EventPublisher::post(std::make_shared<ConstituteCameraFailed>(request->id(), ErrorCode::entityAlreadyExists));
        return;
    }
    auto camera = m_factory->constituteCamera(request->id(), request->dto(), false);
    if (request->persistenceLevel() == PersistenceLevel::Repository)
    {
        std::lock_guard locker{ m_cameraMapLock };
        m_cameras.insert_or_assign(request->id(), camera);
    }
    else if (request->persistenceLevel() == PersistenceLevel::Store)
    {
        putCamera(camera);
    }
    request->setResult(camera);
}

void SceneGraphRepository::putCamera(const std::shared_ptr<Camera>& camera)
{
    assert(m_storeMapper);
    assert(camera);
    std::lock_guard locker{ m_cameraMapLock };
    m_cameras.insert_or_assign(camera->id(), camera);
    auto camera_dto = camera->serializeDto();
    auto er = m_storeMapper->putCamera(camera->id(), { camera_dto });
    if (!er)
    {
        EventPublisher::post(std::make_shared<CameraPut>(camera->id()));
    }
    else
    {
        EventPublisher::post(std::make_shared<PutCameraFailed>(camera->id(), er));
    }
}

void SceneGraphRepository::removeCamera(const SpatialId& id)
{
    if (!hasCamera(id)) return;
    std::lock_guard locker{ m_cameraMapLock };
    m_cameras.erase(id);
    auto er = m_storeMapper->removeCamera(id);
    if (!er)
    {
        EventPublisher::post(std::make_shared<CameraRemoved>(id));
    }
    else
    {
        EventPublisher::post(std::make_shared<RemoveCameraFailed>(id, er));
    }
}

void SceneGraphRepository::putSpatial(const std::shared_ptr<Spatial>& spatial, PersistenceLevel persistence_level)
{
    assert(m_storeMapper);
    assert(spatial);
    std::lock_guard locker{ m_spatialMapLock };
    spatial->persistenceLevel(persistence_level);
    if (persistence_level >= PersistenceLevel::Repository)
    {
        m_spatials.insert_or_assign(spatial->id(), spatial);
    }
    if (persistence_level >= PersistenceLevel::Store)
    {
        auto dto = spatial->serializeDto();
        auto er = m_storeMapper->putSpatial(spatial->id(), { dto });
        if (!er)
        {
            EventPublisher::post(std::make_shared<SpatialPut>(spatial->id()));
        }
        else
        {
            EventPublisher::post(std::make_shared<PutSpatialFailed>(spatial->id(), er));
        }
    }
}

void SceneGraphRepository::putLaziedContent(const std::shared_ptr<LazyNode>& lazy_node)
{
    assert(m_storeMapper);
    assert(lazy_node);
    auto dto = lazy_node->serializeLaziedContent();
    auto er = m_storeMapper->putLaziedContent(lazy_node->id(), dto);
    if (!er)
    {
        EventPublisher::post(std::make_shared<LaziedContentPut>(lazy_node->id()));
    }
    else
    {
        EventPublisher::post(std::make_shared<PutLaziedContentFailed>(lazy_node->id(), er));
    }
}

void SceneGraphRepository::removeSpatial(const SpatialId& id)
{
    if (!hasSpatial(id)) return;
    if (const auto spatial = querySpatial(id)) spatial->persistenceLevel(PersistenceLevel::None);
    std::lock_guard locker{ m_spatialMapLock };
    m_spatials.erase(id);
    auto er = m_storeMapper->removeSpatial(id);
    if (!er)
    {
        EventPublisher::post(std::make_shared<SpatialRemoved>(id));
    }
    else
    {
        EventPublisher::post(std::make_shared<RemoveSpatialFailed>(id, er));
    }
}

void SceneGraphRepository::removeLaziedContent(const SpatialId& id)
{
    if (!hasLaziedContent(id)) return;
    auto er = m_storeMapper->removeLaziedContent(id);
    if (!er)
    {
        EventPublisher::post(std::make_shared<LaziedContentRemoved>(id));
    }
    else
    {
        EventPublisher::post(std::make_shared<RemoveLaziedContentFailed>(id, er));
    }
}

/*void SceneGraphRepository::queryNode(const Frameworks::IQueryPtr& q)
{
    if (!q) return;
    const auto query = std::dynamic_pointer_cast<QueryNode>(q);
    assert(query);
    query->setResult(queryNode(query->nodeName()));
}*/

void SceneGraphRepository::querySpatial(const Frameworks::IQueryPtr& q)
{
    if (!q) return;
    const auto query = std::dynamic_pointer_cast<QuerySpatial>(q);
    assert(query);
    query->setResult(querySpatial(query->id()));
}

void SceneGraphRepository::requestSpatialCreation(const Frameworks::IQueryPtr& r)
{
    assert(m_factory);
    if (!r) return;
    auto request = std::dynamic_pointer_cast<RequestSpatialCreation>(r);
    if (!request) return;
    if (hasSpatial(request->id()))
    {
        EventPublisher::post(std::make_shared<CreateSpatialFailed>(request->id(), ErrorCode::entityAlreadyExists));
        return;
    }
    auto spatial = m_factory->createSpatial(request->id());
    putSpatial(spatial, request->persistenceLevel());
    request->setResult(spatial);
}

void SceneGraphRepository::requestSpatialConstitution(const Frameworks::IQueryPtr& r)
{
    assert(m_factory);
    if (!r) return;
    auto request = std::dynamic_pointer_cast<RequestSpatialConstitution>(r);
    if (!request) return;
    if (hasSpatial(request->id()))
    {
        EventPublisher::post(std::make_shared<ConstituteSpatialFailed>(request->id(), ErrorCode::entityAlreadyExists));
        return;
    }
    auto spatial = m_factory->constituteSpatial(request->id(), request->dto(), false);
    putSpatial(spatial, request->persistenceLevel());
    request->setResult(spatial);
}

void SceneGraphRepository::requestLightCreation(const Frameworks::IQueryPtr& r)
{
    assert(m_factory);
    if (!r) return;
    const auto request = std::dynamic_pointer_cast<RequestLightCreation>(r);
    if (!request) return;
    if (hasSpatial(request->id()))
    {
        EventPublisher::post(std::make_shared<CreateSpatialFailed>(request->id(), ErrorCode::entityAlreadyExists));
        return;
    }
    const auto light = m_factory->createLight(request->id(), request->info());
    putSpatial(light, request->persistenceLevel());
    request->setResult(light);
}

void SceneGraphRepository::putCamera(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<PutCamera>(c);
    assert(cmd);
    putCamera(cmd->camera());
}

void SceneGraphRepository::removeCamera(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<RemoveCamera>(c);
    assert(cmd);
    removeCamera(cmd->id());
}

void SceneGraphRepository::putSpatial(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<PutSpatial>(c);
    assert(cmd);
    putSpatial(cmd->spatial(), PersistenceLevel::Store);
}

void SceneGraphRepository::removeSpatial(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<RemoveSpatial>(c);
    assert(cmd);
    removeSpatial(cmd->id());
}

void SceneGraphRepository::putLaziedContent(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<PutLaziedContent>(c);
    assert(cmd);
    putLaziedContent(cmd->lazyNode());
}

void SceneGraphRepository::hydrateLazyNode(const SpatialId& id)
{
    auto lazy_node = std::dynamic_pointer_cast<LazyNode>(querySpatial(id));
    if (!lazy_node)
    {
        EventPublisher::post(std::make_shared<LazyNodeHydrationFailed>(id, ErrorCode::nodeNotFound));
        return;
    }
    if (!lazy_node->lazyStatus().isInQueue()) return;
    lazy_node->lazyStatus().changeStatus(LazyStatus::Status::Loading);
    const auto content = m_storeMapper->queryLaziedContent(id);
    if (!content)
    {
        lazy_node->lazyStatus().changeStatus(LazyStatus::Status::Failed);
        EventPublisher::post(std::make_shared<LazyNodeHydrationFailed>(id, ErrorCode::laziedContentNotFound));
        return;
    }
    if (error er = lazy_node->hydrate(content.value()))
    {
        EventPublisher::post(std::make_shared<LazyNodeHydrationFailed>(id, er));
    }
    else
    {
        EventPublisher::post(std::make_shared<LazyNodeHydrated>(id));
    }
}

void SceneGraphRepository::removeLaziedContent(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<RemoveLaziedContent>(c);
    assert(cmd);
    removeLaziedContent(cmd->id());
}


void SceneGraphRepository::attachNodeChild(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<AttachNodeChild>(c);
    assert(cmd);
    auto node = std::dynamic_pointer_cast<Node>(querySpatial(cmd->nodeId()));
    if (!node)
    {
        EventPublisher::post(std::make_shared<AttachNodeChildFailed>(cmd->nodeId(), cmd->child()->id(), ErrorCode::nodeNotFound));
        return;
    }
    if (error er = node->attachChild(cmd->child(), cmd->localTransform()))
    {
        EventPublisher::post(std::make_shared<AttachNodeChildFailed>(cmd->nodeId(), cmd->child()->id(), er));
    }
    else
    {
        EventPublisher::post(std::make_shared<NodeChildAttached>(cmd->nodeId(), cmd->child()));
    }
}

