#include "WorldMapRepository.h"
#include "WorldMapStoreMapper.h"
#include "WorldMapQueries.h"
#include "QuadTreeRoot.h"
#include "WorldMapErrors.h"
#include "WorldMapEvents.h"
#include "Frameworks/QueryDispatcher.h"
#include "Frameworks/EventPublisher.h"

using namespace Enigma::WorldMap;
using namespace Enigma::Frameworks;

DEFINE_RTTI(WorldMap, WorldMapRepository, ISystemService);

WorldMapRepository::WorldMapRepository(Frameworks::ServiceManager* srv_manager, const std::shared_ptr<WorldMapStoreMapper>& world_map_store_mapper) : ISystemService(srv_manager)
{
    m_storeMapper = world_map_store_mapper;
}

WorldMapRepository::~WorldMapRepository()
{
}

ServiceResult WorldMapRepository::onInit()
{
    m_storeMapper->connect();

    m_queryQuadTreeRoot = std::make_shared<QuerySubscriber>([=](const IQueryPtr& q) { queryQuadTreeRoot(q); });
    QueryDispatcher::subscribe(typeid(QueryQuadTreeRoot), m_queryQuadTreeRoot);
    m_hasQuadTreeRoot = std::make_shared<QuerySubscriber>([=](const IQueryPtr& q) { hasQuadTreeRoot(q); });
    QueryDispatcher::subscribe(typeid(HasQuadTreeRoot), m_hasQuadTreeRoot);
    m_requestQuadTreeRootCreation = std::make_shared<QuerySubscriber>([=](const IQueryPtr& q) { requestQuadTreeRootCreation(q); });
    QueryDispatcher::subscribe(typeid(RequestQuadTreeRootCreation), m_requestQuadTreeRootCreation);
    m_requestQuadTreeRootConstitution = std::make_shared<QuerySubscriber>([=](const IQueryPtr& q) { requestQuadTreeRootConstitution(q); });
    QueryDispatcher::subscribe(typeid(RequestQuadTreeRootConstitution), m_requestQuadTreeRootConstitution);

    m_queryWorldMap = std::make_shared<QuerySubscriber>([=](const IQueryPtr& q) { queryWorldMap(q); });
    QueryDispatcher::subscribe(typeid(QueryWorldMap), m_queryWorldMap);
    m_hasWorldMap = std::make_shared<QuerySubscriber>([=](const IQueryPtr& q) { hasWorldMap(q); });
    QueryDispatcher::subscribe(typeid(HasWorldMap), m_hasWorldMap);
    m_requestWorldMapCreation = std::make_shared<QuerySubscriber>([=](const IQueryPtr& q) { requestWorldMapCreation(q); });
    QueryDispatcher::subscribe(typeid(RequestWorldMapCreation), m_requestWorldMapCreation);
    m_requestWorldMapConstitution = std::make_shared<QuerySubscriber>([=](const IQueryPtr& q) { requestWorldMapConstitution(q); });
    QueryDispatcher::subscribe(typeid(RequestWorldMapConstitution), m_requestWorldMapConstitution);

    return ServiceResult::Complete;
}

ServiceResult WorldMapRepository::onTerm()
{
    QueryDispatcher::unsubscribe(typeid(QueryQuadTreeRoot), m_queryQuadTreeRoot);
    m_queryQuadTreeRoot = nullptr;
    QueryDispatcher::unsubscribe(typeid(HasQuadTreeRoot), m_hasQuadTreeRoot);
    m_hasQuadTreeRoot = nullptr;
    QueryDispatcher::unsubscribe(typeid(RequestQuadTreeRootCreation), m_requestQuadTreeRootCreation);
    m_requestQuadTreeRootCreation = nullptr;
    QueryDispatcher::unsubscribe(typeid(RequestQuadTreeRootConstitution), m_requestQuadTreeRootConstitution);
    m_requestQuadTreeRootConstitution = nullptr;

    QueryDispatcher::unsubscribe(typeid(QueryWorldMap), m_queryWorldMap);
    m_queryWorldMap = nullptr;
    QueryDispatcher::unsubscribe(typeid(HasWorldMap), m_hasWorldMap);
    m_hasWorldMap = nullptr;
    QueryDispatcher::unsubscribe(typeid(RequestWorldMapCreation), m_requestWorldMapCreation);
    m_requestWorldMapCreation = nullptr;
    QueryDispatcher::unsubscribe(typeid(RequestWorldMapConstitution), m_requestWorldMapConstitution);
    m_requestWorldMapConstitution = nullptr;

    m_storeMapper->disconnect();

    return ServiceResult::Complete;
}

bool WorldMapRepository::hasQuadTreeRoot(const QuadTreeRootId& id)
{
    assert(m_storeMapper);
    std::lock_guard locker{ m_quadTreeRootLock };
    auto it = m_quadTreeRoots.find(id);
    if (it != m_quadTreeRoots.end()) return true;
    return m_storeMapper->hasQuadTreeRoot(id);
}

std::shared_ptr<QuadTreeRoot> WorldMapRepository::queryQuadTreeRoot(const QuadTreeRootId& id)
{
    assert(m_storeMapper);
    std::lock_guard locker{ m_quadTreeRootLock };
    auto it = m_quadTreeRoots.find(id);
    if (it != m_quadTreeRoots.end()) return it->second;
    auto dto = m_storeMapper->queryQuadTreeRoot(id);
    if (!dto) return nullptr;
    auto quadTreeRoot = std::make_shared<QuadTreeRoot>(id, *dto);
    m_quadTreeRoots[id] = quadTreeRoot;
    return quadTreeRoot;
}

void WorldMapRepository::putQuadTreeRoot(const QuadTreeRootId& id, const std::shared_ptr<QuadTreeRoot>& quad_tree_root, PersistenceLevel persistence_level)
{
    assert(m_storeMapper);
    assert(quad_tree_root);
    std::lock_guard locker{ m_quadTreeRootLock };
    if (persistence_level >= PersistenceLevel::Repository)
    {
        m_quadTreeRoots[id] = quad_tree_root;
    }
    if (persistence_level >= PersistenceLevel::Store)
    {
        auto er = m_storeMapper->putQuadTreeRoot(id, quad_tree_root->serializeDto());
        if (!er)
        {
            EventPublisher::enqueue(std::make_shared<QuadTreeRootPut>(id));
        }
        else
        {
            EventPublisher::enqueue(std::make_shared<PutQuadTreeRootFailed>(id, er));
        }
    }
}

void WorldMapRepository::removeQuadTreeRoot(const QuadTreeRootId& id)
{
    if (!hasQuadTreeRoot(id)) return;
    assert(m_storeMapper);
    std::lock_guard locker{ m_quadTreeRootLock };
    m_quadTreeRoots.erase(id);
    auto er = m_storeMapper->removeQuadTreeRoot(id);
    if (!er)
    {
        EventPublisher::enqueue(std::make_shared<QuadTreeRootRemoved>(id));
    }
    else
    {
        EventPublisher::enqueue(std::make_shared<RemoveQuadTreeRootFailed>(id, er));
    }
}

bool WorldMapRepository::hasWorldMap(const WorldMapId& id)
{
    assert(m_storeMapper);
    std::lock_guard locker{ m_worldMapLock };
    auto it = m_worldMaps.find(id);
    if (it != m_worldMaps.end()) return true;
    return m_storeMapper->hasWorldMap(id);
}

std::shared_ptr<WorldMap> WorldMapRepository::queryWorldMap(const WorldMapId& id)
{
    assert(m_storeMapper);
    std::lock_guard locker{ m_worldMapLock };
    auto it = m_worldMaps.find(id);
    if (it != m_worldMaps.end()) return it->second;
    auto dto = m_storeMapper->queryWorldMap(id);
    if (!dto) return nullptr;
    auto worldMap = std::make_shared<WorldMap>(id, *dto);
    m_worldMaps[id] = worldMap;
    return worldMap;
}

void WorldMapRepository::putWorldMap(const WorldMapId& id, const std::shared_ptr<WorldMap>& world_map, PersistenceLevel persistence_level)
{
    assert(m_storeMapper);
    assert(world_map);
    std::lock_guard locker{ m_worldMapLock };
    if (persistence_level >= PersistenceLevel::Repository)
    {
        m_worldMaps[id] = world_map;
    }
    if (persistence_level >= PersistenceLevel::Store)
    {
        auto er = m_storeMapper->putWorldMap(id, world_map->serializeDto());
        if (!er)
        {
            EventPublisher::enqueue(std::make_shared<WorldMapPut>(id));
        }
        else
        {
            EventPublisher::enqueue(std::make_shared<PutWorldMapFailed>(id, er));
        }
    }
}

void WorldMapRepository::removeWorldMap(const WorldMapId& id)
{
    if (!hasWorldMap(id)) return;
    assert(m_storeMapper);
    std::lock_guard locker{ m_worldMapLock };
    m_worldMaps.erase(id);
    auto er = m_storeMapper->removeWorldMap(id);
    if (!er)
    {
        EventPublisher::enqueue(std::make_shared<WorldMapRemoved>(id));
    }
    else
    {
        EventPublisher::enqueue(std::make_shared<RemoveWorldMapFailed>(id, er));
    }
}

void WorldMapRepository::queryQuadTreeRoot(const Frameworks::IQueryPtr q)
{
    auto query = std::dynamic_pointer_cast<QueryQuadTreeRoot>(q);
    assert(query);
    auto quadTreeRoot = queryQuadTreeRoot(query->id());
    query->setResult(quadTreeRoot);
}

void WorldMapRepository::hasQuadTreeRoot(const Frameworks::IQueryPtr q)
{
    auto query = std::dynamic_pointer_cast<HasQuadTreeRoot>(q);
    assert(query);
    query->setResult(hasQuadTreeRoot(query->id()));
}

void WorldMapRepository::requestQuadTreeRootCreation(const Frameworks::IQueryPtr q)
{
    auto request = std::dynamic_pointer_cast<RequestQuadTreeRootCreation>(q);
    assert(request);
    if (hasQuadTreeRoot(request->id()))
    {
        EventPublisher::enqueue(std::make_shared<QuadTreeRootCreationFailed>(request->id(), ErrorCode::quadRootAlreadyExists));
        return;
    }
    auto quadTreeRoot = std::make_shared<QuadTreeRoot>(request->id());
    putQuadTreeRoot(request->id(), quadTreeRoot, request->persistenceLevel());
    request->setResult(quadTreeRoot);
}

void WorldMapRepository::requestQuadTreeRootConstitution(const Frameworks::IQueryPtr q)
{
    auto request = std::dynamic_pointer_cast<RequestQuadTreeRootConstitution>(q);
    assert(request);
    if (hasQuadTreeRoot(request->id()))
    {
        EventPublisher::enqueue(std::make_shared<QuadTreeRootConstitutionFailed>(request->id(), ErrorCode::quadRootAlreadyExists));
        return;
    }
    auto quadTreeRoot = std::make_shared<QuadTreeRoot>(request->id(), request->dto());
    putQuadTreeRoot(request->id(), quadTreeRoot, request->persistenceLevel());
    request->setResult(quadTreeRoot);
}

void WorldMapRepository::queryWorldMap(const Frameworks::IQueryPtr q)
{
    auto query = std::dynamic_pointer_cast<QueryWorldMap>(q);
    assert(query);
    auto worldMap = queryWorldMap(query->id());
    query->setResult(worldMap);
}

void WorldMapRepository::hasWorldMap(const Frameworks::IQueryPtr q)
{
    auto query = std::dynamic_pointer_cast<HasWorldMap>(q);
    assert(query);
    query->setResult(hasWorldMap(query->id()));
}

void WorldMapRepository::requestWorldMapCreation(const Frameworks::IQueryPtr q)
{
    auto request = std::dynamic_pointer_cast<RequestWorldMapCreation>(q);
    assert(request);
    if (hasWorldMap(request->id()))
    {
        EventPublisher::enqueue(std::make_shared<WorldMapCreationFailed>(request->id(), ErrorCode::worldMapAlreadyExists));
        return;
    }
    auto worldMap = std::make_shared<WorldMap>(request->id());
    putWorldMap(request->id(), worldMap, request->persistenceLevel());
    request->setResult(worldMap);
}

void WorldMapRepository::requestWorldMapConstitution(const Frameworks::IQueryPtr q)
{
    auto request = std::dynamic_pointer_cast<RequestWorldMapConstitution>(q);
    assert(request);
    if (hasWorldMap(request->id()))
    {
        EventPublisher::enqueue(std::make_shared<WorldMapConstitutionFailed>(request->id(), ErrorCode::worldMapAlreadyExists));
        return;
    }
    auto worldMap = std::make_shared<WorldMap>(request->id(), request->dto());
    putWorldMap(request->id(), worldMap, request->persistenceLevel());
    request->setResult(worldMap);
}
