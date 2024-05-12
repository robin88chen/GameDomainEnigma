#include "WorldMapRepository.h"
#include "WorldMapStoreMapper.h"
#include "WorldMapQueries.h"
#include "Frameworks/QueryDispatcher.h"

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
    m_queryQuadTreeRoot = std::make_shared<QuerySubscriber>([=](const IQueryPtr& q) { queryQuadTreeRoot(q); });
    QueryDispatcher::subscribe(typeid(QueryQuadTreeRoot), m_queryQuadTreeRoot);
    return ServiceResult::Complete;
}

ServiceResult WorldMapRepository::onTerm()
{
    QueryDispatcher::unsubscribe(typeid(QueryQuadTreeRoot), m_queryQuadTreeRoot);
    m_queryQuadTreeRoot = nullptr;
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

void WorldMapRepository::queryQuadTreeRoot(const Frameworks::IQueryPtr q)
{
    auto query = std::dynamic_pointer_cast<QueryQuadTreeRoot>(q);
    assert(query);
    auto quadTreeRoot = queryQuadTreeRoot(query->id());
    query->setResult(quadTreeRoot);
}

