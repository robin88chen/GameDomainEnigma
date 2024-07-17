#include "SceneGraphRepository.h"
#include "SceneGraphStoreMapper.h"
#include "SceneGraphFactory.h"
#include "SpatialId.h"
#include "Camera.h"
#include "Frustum.h"
#include "Node.h"
#include "Pawn.h"
#include "LazyNode.h"
#include "SceneGraphEvents.h"
#include "SceneGraphCommands.h"
#include "SceneGraphErrors.h"
#include "Portal.h"
#include "Light.h"
#include "Frameworks/EventPublisher.h"
#include "Frameworks/CommandBus.h"
#include "Frameworks/QueryDispatcher.h"
#include "Platforms/PlatformLayerUtilities.h"
#include "Platforms/MemoryMacro.h"
#include "CameraFrustumCommands.h"
#include "CameraFrustumEvents.h"
#include "SceneGraphQueries.h"
#include "SceneGraphDtos.h"
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
    registerHandlers();
}

SceneGraphRepository::~SceneGraphRepository()
{
    unregisterHandlers();
    SAFE_DELETE(m_factory);
}

ServiceResult SceneGraphRepository::onInit()
{
    m_storeMapper->connect();
    return ServiceResult::Complete;
}
ServiceResult SceneGraphRepository::onTerm()
{
    dumpRetainedCameras();
    dumpRetainedSpatials();

    m_storeMapper->disconnect();

    m_cameras.clear();
    m_spatials.clear();

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
    m_querySpatial = std::make_shared<QuerySubscriber>([=](const IQueryPtr& q) { querySpatial(q); });
    QueryDispatcher::subscribe(typeid(QuerySpatial), m_querySpatial);
    m_hasSpatial = std::make_shared<QuerySubscriber>([=](const IQueryPtr& q) { hasSpatial(q); });
    QueryDispatcher::subscribe(typeid(HasSpatial), m_hasSpatial);
    m_requestSpatialCreation = std::make_shared<QuerySubscriber>([=](const IQueryPtr& q) { requestSpatialCreation(q); });
    QueryDispatcher::subscribe(typeid(RequestSpatialCreation), m_requestSpatialCreation);
    m_requestSpatialConstitution = std::make_shared<QuerySubscriber>([=](const IQueryPtr& q) { requestSpatialConstitution(q); });
    QueryDispatcher::subscribe(typeid(RequestSpatialConstitution), m_requestSpatialConstitution);
    m_requestLightCreation = std::make_shared<QuerySubscriber>([=](const IQueryPtr& q) { requestLightCreation(q); });
    QueryDispatcher::subscribe(typeid(RequestLightCreation), m_requestLightCreation);
    m_queryWorldTransform = std::make_shared<QuerySubscriber>([=](const IQueryPtr& q) { queryWorldTransform(q); });
    QueryDispatcher::subscribe(typeid(QueryWorldTransform), m_queryWorldTransform);
    m_queryModelBound = std::make_shared<QuerySubscriber>([=](const IQueryPtr& q) { queryModelBound(q); });
    QueryDispatcher::subscribe(typeid(QueryModelBound), m_queryModelBound);
    m_queryRunningSpatial = std::make_shared<QuerySubscriber>([=](const IQueryPtr& q) { queryRunningSpatial(q); });
    QueryDispatcher::subscribe(typeid(QueryRunningSpatial), m_queryRunningSpatial);

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
}

void SceneGraphRepository::unregisterHandlers()
{
    QueryDispatcher::unsubscribe(typeid(QueryCamera), m_queryCamera);
    m_queryCamera = nullptr;
    QueryDispatcher::unsubscribe(typeid(RequestCameraCreation), m_requestCameraCreation);
    m_requestCameraCreation = nullptr;
    QueryDispatcher::unsubscribe(typeid(RequestCameraConstitution), m_requestCameraConstitution);
    m_requestCameraConstitution = nullptr;
    QueryDispatcher::unsubscribe(typeid(QuerySpatial), m_querySpatial);
    m_querySpatial = nullptr;
    QueryDispatcher::unsubscribe(typeid(HasSpatial), m_hasSpatial);
    m_hasSpatial = nullptr;
    QueryDispatcher::unsubscribe(typeid(RequestSpatialCreation), m_requestSpatialCreation);
    m_requestSpatialCreation = nullptr;
    QueryDispatcher::unsubscribe(typeid(RequestSpatialConstitution), m_requestCameraConstitution);
    m_requestSpatialConstitution = nullptr;
    QueryDispatcher::unsubscribe(typeid(RequestLightCreation), m_requestLightCreation);
    m_requestLightCreation = nullptr;
    QueryDispatcher::unsubscribe(typeid(QueryWorldTransform), m_queryWorldTransform);
    m_queryWorldTransform = nullptr;
    QueryDispatcher::unsubscribe(typeid(QueryModelBound), m_queryModelBound);
    m_queryModelBound = nullptr;
    QueryDispatcher::unsubscribe(typeid(QueryRunningSpatial), m_queryRunningSpatial);
    m_queryRunningSpatial = nullptr;

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
}

void SceneGraphRepository::setCoordinateSystem(GraphicCoordSys hand)
{
    m_handSystem = hand;
}

GraphicCoordSys SceneGraphRepository::getCoordinateSystem()
{
    return m_handSystem;
}

void SceneGraphRepository::registerSpatialFactory(const std::string& rtti, const SpatialCreator& creator, const SpatialConstitutor& constitutor)
{
    assert(m_factory);
    m_factory->registerSpatialFactory(rtti, creator, constitutor);
}

void SceneGraphRepository::registerSpatialLightFactory(const std::string& rtti, const LightCreator& creator, const LightConstitutor& constitutor)
{
    assert(m_factory);
    m_factory->registerLightFactory(rtti, creator, constitutor);
}

bool SceneGraphRepository::hasCamera(const SpatialId& id)
{
    assert(m_storeMapper);
    if (findCachedCamera(id)) return true;
    return m_storeMapper->hasCamera(id);
}

std::shared_ptr<Camera> SceneGraphRepository::queryCamera(const SpatialId& id)
{
    if (!hasCamera(id)) return nullptr;
    if (auto camera = findCachedCamera(id); camera) return camera;
    std::lock_guard locker{ m_cameraMapLock };
    auto dto = m_storeMapper->queryCamera(id);
    assert(dto.has_value());
    auto camera = m_factory->constituteCamera(id, dto.value(), true);
    assert(camera);
    m_cameras.insert_or_assign(id, camera);
    return camera;
}

bool SceneGraphRepository::hasSpatial(const SpatialId& id)
{
    assert(m_storeMapper);
    assert(id.rtti().isDerived(Spatial::TYPE_RTTI));
    if (findCachedSpatial(id)) return true;
    return m_storeMapper->hasSpatial(id);
}

std::shared_ptr<Spatial> SceneGraphRepository::querySpatial(const SpatialId& id)
{
    if (!hasSpatial(id)) return nullptr;
    if (auto spatial = findCachedSpatial(id); spatial) return spatial;
    std::lock_guard locker{ m_spatialMapLock };
    auto dto = m_storeMapper->querySpatial(id);
    assert(dto.has_value());
    std::shared_ptr<Spatial> spatial;
    if (id.rtti().isDerived(Light::TYPE_RTTI))
    {
        spatial = m_factory->constituteLight(id, dto.value(), true);
    }
    else
    {
        spatial = m_factory->constituteSpatial(id, dto.value(), true);
    }
    assert(spatial);
    m_spatials.insert_or_assign(id, spatial);
    return spatial;
}

bool SceneGraphRepository::hasLaziedContent(const SpatialId& id)
{
    assert(m_storeMapper);
    assert(id.rtti().isDerived(LazyNode::TYPE_RTTI));
    return m_storeMapper->hasLaziedContent(id);
}

Enigma::MathLib::Matrix4 SceneGraphRepository::queryWorldTransform(const SpatialId& id)
{
    if (!hasSpatial(id)) return MathLib::Matrix4::ZERO;
    if (auto spatial = querySpatial(id); spatial) return spatial->getWorldTransform();
    auto dto = m_storeMapper->querySpatial(id);
    assert(dto.has_value());
    SpatialDto spatial_dto = SpatialDto(dto.value());
    return spatial_dto.worldTransform();
}

Enigma::Engine::BoundingVolume SceneGraphRepository::queryModelBound(const SpatialId& id)
{
    if (!hasSpatial(id)) return BoundingVolume{};
    if (auto spatial = querySpatial(id); spatial) return spatial->getModelBound();
    auto dto = m_storeMapper->querySpatial(id);
    assert(dto.has_value());
    SpatialDto spatial_dto = SpatialDto(dto.value());
    BoundingVolumeDto bv_dto = BoundingVolumeDto::fromGenericDto(spatial_dto.modelBound());
    if (bv_dto.box()) return BoundingVolume{ bv_dto.box().value() };
    if (bv_dto.sphere()) return BoundingVolume{ bv_dto.sphere().value() };
    return BoundingVolume{};
}

std::shared_ptr<Spatial> SceneGraphRepository::queryRunningSpatial(const SpatialId& id)
{
    return findCachedSpatial(id);
}

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
        EventPublisher::enqueue(std::make_shared<CreateCameraFailed>(request->id(), ErrorCode::entityAlreadyExists));
        return;
    }
    auto camera = m_factory->createCamera(request->id());
    assert(camera);
    std::lock_guard locker{ m_cameraMapLock };
    m_cameras.insert_or_assign(request->id(), camera);
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
        EventPublisher::enqueue(std::make_shared<ConstituteCameraFailed>(request->id(), ErrorCode::entityAlreadyExists));
        return;
    }
    auto camera = m_factory->constituteCamera(request->id(), request->dto(), false);
    assert(camera);
    std::lock_guard locker{ m_cameraMapLock };
    m_cameras.insert_or_assign(request->id(), camera);
    request->setResult(camera);
}

void SceneGraphRepository::putCamera(const std::shared_ptr<Camera>& camera)
{
    assert(m_storeMapper);
    assert(camera);
    auto camera_dto = camera->serializeDto();
    auto er = m_storeMapper->putCamera(camera->id(), { camera_dto });
    if (!er)
    {
        EventPublisher::enqueue(std::make_shared<CameraPut>(camera->id()));
    }
    else
    {
        EventPublisher::enqueue(std::make_shared<PutCameraFailed>(camera->id(), er));
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
        EventPublisher::enqueue(std::make_shared<CameraRemoved>(id));
    }
    else
    {
        EventPublisher::enqueue(std::make_shared<RemoveCameraFailed>(id, er));
    }
}

void SceneGraphRepository::putSpatial(const std::shared_ptr<Spatial>& spatial)
{
    assert(m_storeMapper);
    assert(spatial);
    std::lock_guard locker{ m_spatialMapLock };
    spatial->persistenceLevel(PersistenceLevel::Store);
    auto dto = spatial->serializeDto();
    auto er = m_storeMapper->putSpatial(spatial->id(), { dto });
    if (!er)
    {
        EventPublisher::enqueue(std::make_shared<SpatialPut>(spatial->id()));
    }
    else
    {
        EventPublisher::enqueue(std::make_shared<PutSpatialFailed>(spatial->id(), er));
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
        EventPublisher::enqueue(std::make_shared<LaziedContentPut>(lazy_node->id()));
    }
    else
    {
        EventPublisher::enqueue(std::make_shared<PutLaziedContentFailed>(lazy_node->id(), er));
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
        EventPublisher::enqueue(std::make_shared<SpatialRemoved>(id));
    }
    else
    {
        EventPublisher::enqueue(std::make_shared<RemoveSpatialFailed>(id, er));
    }
}

void SceneGraphRepository::removeLaziedContent(const SpatialId& id)
{
    if (!hasLaziedContent(id)) return;
    auto er = m_storeMapper->removeLaziedContent(id);
    if (!er)
    {
        EventPublisher::enqueue(std::make_shared<LaziedContentRemoved>(id));
    }
    else
    {
        EventPublisher::enqueue(std::make_shared<RemoveLaziedContentFailed>(id, er));
    }
}

void SceneGraphRepository::querySpatial(const Frameworks::IQueryPtr& q)
{
    if (!q) return;
    const auto query = std::dynamic_pointer_cast<QuerySpatial>(q);
    assert(query);
    query->setResult(querySpatial(query->id()));
}

void SceneGraphRepository::hasSpatial(const Frameworks::IQueryPtr& q)
{
    if (!q) return;
    const auto query = std::dynamic_pointer_cast<HasSpatial>(q);
    assert(query);
    query->setResult(hasSpatial(query->id()));
}

void SceneGraphRepository::queryWorldTransform(const Frameworks::IQueryPtr& q)
{
    if (!q) return;
    const auto query = std::dynamic_pointer_cast<QueryWorldTransform>(q);
    assert(query);
    query->setResult(queryWorldTransform(query->id()));
}

void SceneGraphRepository::queryModelBound(const Frameworks::IQueryPtr& q)
{
    if (!q) return;
    const auto query = std::dynamic_pointer_cast<QueryModelBound>(q);
    assert(query);
    query->setResult(queryModelBound(query->id()));
}

void SceneGraphRepository::queryRunningSpatial(const Frameworks::IQueryPtr& q)
{
    if (!q) return;
    const auto query = std::dynamic_pointer_cast<QueryRunningSpatial>(q);
    assert(query);
    query->setResult(queryRunningSpatial(query->id()));
}

void SceneGraphRepository::requestSpatialCreation(const Frameworks::IQueryPtr& r)
{
    assert(m_factory);
    if (!r) return;
    auto request = std::dynamic_pointer_cast<RequestSpatialCreation>(r);
    if (!request) return;
    if (hasSpatial(request->id()))
    {
        EventPublisher::enqueue(std::make_shared<CreateSpatialFailed>(request->id(), ErrorCode::entityAlreadyExists));
        return;
    }
    auto spatial = m_factory->createSpatial(request->id());
    assert(spatial);
    std::lock_guard locker{ m_spatialMapLock };
    spatial->persistenceLevel(PersistenceLevel::Repository);
    m_spatials.insert_or_assign(request->id(), spatial);
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
        EventPublisher::enqueue(std::make_shared<ConstituteSpatialFailed>(request->id(), ErrorCode::entityAlreadyExists));
        return;
    }
    auto spatial = m_factory->constituteSpatial(request->id(), request->dto(), false);
    assert(spatial);
    std::lock_guard locker{ m_spatialMapLock };
    spatial->persistenceLevel(PersistenceLevel::Repository);
    m_spatials.insert_or_assign(request->id(), spatial);
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
        EventPublisher::enqueue(std::make_shared<CreateSpatialFailed>(request->id(), ErrorCode::entityAlreadyExists));
        return;
    }
    const auto light = m_factory->createLight(request->id(), request->info());
    assert(light);
    std::lock_guard locker{ m_spatialMapLock };
    light->persistenceLevel(PersistenceLevel::Repository);
    m_spatials.insert_or_assign(request->id(), light);
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
    putSpatial(cmd->spatial());
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
        EventPublisher::enqueue(std::make_shared<LazyNodeHydrationFailed>(id, ErrorCode::nodeNotFound));
        return;
    }
    if (!lazy_node->lazyStatus().isInQueue()) return;
    lazy_node->lazyStatus().changeStatus(LazyStatus::Status::Loading);
    const auto content = m_storeMapper->queryLaziedContent(id);
    if (!content)
    {
        lazy_node->lazyStatus().changeStatus(LazyStatus::Status::Failed);
        EventPublisher::enqueue(std::make_shared<LazyNodeHydrationFailed>(id, ErrorCode::laziedContentNotFound));
        return;
    }
    if (error er = lazy_node->hydrate(content.value()))
    {
        EventPublisher::enqueue(std::make_shared<LazyNodeHydrationFailed>(id, er));
    }
    else
    {
        EventPublisher::enqueue(std::make_shared<LazyNodeHydrated>(id));
    }
}

void SceneGraphRepository::removeLaziedContent(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<RemoveLaziedContent>(c);
    assert(cmd);
    removeLaziedContent(cmd->id());
}

std::shared_ptr<Camera> SceneGraphRepository::findCachedCamera(const SpatialId& id)
{
    std::lock_guard locker{ m_cameraMapLock };
    auto it = m_cameras.find(id);
    if ((it != m_cameras.end()) && (!it->second.expired())) return it->second.lock();
    return nullptr;
}

std::shared_ptr<Spatial> SceneGraphRepository::findCachedSpatial(const SpatialId& id)
{
    std::lock_guard locker{ m_spatialMapLock };
    auto it = m_spatials.find(id);
    if ((it != m_spatials.end()) && (!it->second.expired())) return it->second.lock();
    return nullptr;
}

void SceneGraphRepository::dumpRetainedCameras()
{
    Platforms::Debug::Printf("Dumping retained cameras\n");
    std::lock_guard locker{ m_cameraMapLock };
    for (auto& [id, camera] : m_cameras)
    {
        if (camera.expired()) continue;
        Platforms::Debug::Printf("Camera %s is retained\n", id.name().c_str());
    }
}

void SceneGraphRepository::dumpRetainedSpatials()
{
    Platforms::Debug::Printf("Dumping retained spatials\n");
    std::lock_guard locker{ m_spatialMapLock };
    for (auto& [id, spatial] : m_spatials)
    {
        if (spatial.expired()) continue;
        Platforms::Debug::Printf("Spatial %s of %s is retained\n", id.name().c_str(), id.rtti().getName().c_str());
    }
}


