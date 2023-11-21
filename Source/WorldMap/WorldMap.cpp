#include "WorldMap.h"
#include "SceneGraph/PortalZoneNode.h"
#include "WorldMapDto.h"
#include "SceneQuadTreeRoot.h"
#include "SceneGraph/VisibilityManagedNode.h"
#include "SceneGraph/EnumNonDerivedSpatials.h"
#include "Frameworks/LazyStatus.h"
#include "Frameworks/CommandBus.h"
#include "Platforms/PlatformLayer.h"
#include <cassert>

using namespace Enigma::WorldMap;
using namespace Enigma::SceneGraph;
using namespace Enigma::MathLib;
using namespace Enigma::Engine;
using namespace Enigma::Terrain;
using namespace Enigma::Frameworks;

DEFINE_RTTI_OF_BASE(WorldMap, WorldMap);

std::string QUADROOT_POSTFIX = "_qroot";
std::string WOLRD_PORTAL_ROOT_GRAPH = "portal_root_graph";

WorldMap::WorldMap(const std::string& name, const std::shared_ptr<PortalZoneNode>& root) : m_factory_desc(TYPE_RTTI.getName())
{
    m_name = name;
    m_root = root;
    m_factory_desc = m_factory_desc.ClaimAsInstanced(name + ".wld");
}

WorldMap::WorldMap(const std::shared_ptr<SceneGraph::SceneGraphRepository>& repository, const Engine::GenericDto& o) : m_factory_desc(TYPE_RTTI.getName())
{
    WorldMapDto world = WorldMapDto::fromGenericDto(o);
    m_name = world.name();
    m_factory_desc = world.factoryDesc();
    for (auto& quad : world.quadTreeRoots())
    {
        auto quad_root = std::make_shared<SceneQuadTreeRoot>(repository, quad);
        m_listQuadRoot.push_back(quad_root);
    }
    m_root = std::dynamic_pointer_cast<PortalZoneNode>(repository->createNode(world.portalRoot()));
    assert(!m_root.expired());
}

WorldMap::~WorldMap()
{
    m_listQuadRoot.clear();
}

GenericDto WorldMap::serializeDto()
{
    WorldMapDto world_dto;
    world_dto.factoryDesc() = m_factory_desc;
    for (auto& root : m_listQuadRoot)
    {
        if (!root) continue;
        world_dto.quadTreeRoots().emplace_back(root->serializeDto());
    }
    if (!m_root.expired()) world_dto.portalRoot() = m_root.lock()->serializeAsLaziness();
    return world_dto.toGenericDto();
}

std::vector<GenericDtoCollection> WorldMap::serializeSceneGraphs()
{
    assert(!m_root.expired());
    std::vector<Engine::GenericDtoCollection> collections;
    collections.emplace_back(m_root.lock()->serializeFlattenedTree());
    if (m_listQuadRoot.empty()) return collections;
    for (auto& root : m_listQuadRoot)
    {
        if (!root) continue;
        auto root_collection = root->serializeTreeGraphs();
        collections.insert(collections.end(), root_collection.begin(), root_collection.end());
    }
    return collections;
}

void WorldMap::attachTerrain(const std::shared_ptr<SceneGraph::SceneGraphRepository>& repository, const std::shared_ptr<Terrain::TerrainPawn>& terrain, const MathLib::Matrix4& local_transform)
{
    assert(!m_root.expired());
    assert(terrain);
    assert(repository);
    std::string node_name = terrain->getSpatialName() + QUADROOT_POSTFIX; // +NODE_FILE_EXT;
    auto quadRootNode = std::dynamic_pointer_cast<VisibilityManagedNode, Node>(repository->createNode(node_name, VisibilityManagedNode::TYPE_RTTI));
    quadRootNode->lazyStatus().changeStatus(LazyStatus::Status::Ready);
    quadRootNode->factoryDesc().ClaimAsInstanced(node_name + ".node");
    quadRootNode->AttachChild(terrain, Matrix4::IDENTITY);
    m_root.lock()->AttachChild(quadRootNode, local_transform);
    auto treeRoot = std::make_shared<SceneQuadTreeRoot>(node_name, quadRootNode);
    m_listQuadRoot.push_back(treeRoot);
}

std::shared_ptr<Node> WorldMap::queryFittingNode(const Engine::BoundingVolume& bv_in_world) const
{
    if (m_listQuadRoot.empty()) return m_root.lock();
    for (auto root : m_listQuadRoot)
    {
        if (!root) continue;
        if (auto node = root->queryFittingNode(bv_in_world)) return node;
    }
    return m_root.lock();
}
