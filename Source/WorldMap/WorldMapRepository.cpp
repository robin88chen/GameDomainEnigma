#include "WorldMapRepository.h"
#include "WorldMapStoreMapper.h"

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
