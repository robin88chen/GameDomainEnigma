#include "WorldMapService.h"
#include "WorldMap.h"
#include "Frameworks/CommandBus.h"
#include "Frameworks/EventPublisher.h"
#include "Frameworks/QueryDispatcher.h"
#include "WorldMapCommands.h"
#include "WorldMapEvents.h"
#include "WorldMapQueries.h"
#include "SceneGraph/SceneGraphCommands.h"
#include "SceneGraph/SceneGraphEvents.h"
#include "Frameworks/LazyStatus.h"
#include "Platforms/PlatformLayer.h"
#include "SceneGraph/PortalCommands.h"
#include "SceneGraph/VisibilityManagedNode.h"
#include "SceneGraph/EnumDerivedSpatials.h"
#include "SceneGraph/EnumNonDerivedSpatials.h"
#include "SceneGraph/SceneFlattenTraversal.h"
#include "SceneGraph/Spatial.h"
#include "Frameworks/StringFormat.h"
#include "SceneGraph/FindSpatialByName.h"
#include "MathLib/Box2.h"
#include "MathLib/ContainmentBox2.h"

using namespace Enigma::WorldMap;
using namespace Enigma::Frameworks;
using namespace Enigma::SceneGraph;
using namespace Enigma::Terrain;
using namespace Enigma::MathLib;

DEFINE_RTTI(WorldMap, WorldMapService, ISystemService);

std::string WORLD_MAP_TAG = "world_map";
std::string QUADROOT_POSTFIX = "_qroot";

constexpr int MAX_RECURSIVE_DEPTH = 4;

WorldMapService::WorldMapService(ServiceManager* mngr, const std::shared_ptr<SceneGraphRepository>& scene_graph_repository) : ISystemService(mngr)
{
    m_sceneGraphRepository = scene_graph_repository;
    m_needTick = false;
}

WorldMapService::~WorldMapService()
{

}

ServiceResult WorldMapService::onInit()
{
    m_createWorldMap = std::make_shared<CommandSubscriber>([=](auto c) { createEmptyWorldMap(c); });
    CommandBus::subscribe(typeid(CreateEmptyWorldMap), m_createWorldMap);
    m_deserializeWorldMap = std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { deserializeWorldMap(c); });
    CommandBus::subscribe(typeid(DeserializeWorldMap), m_deserializeWorldMap);
    m_attachTerrain = std::make_shared<CommandSubscriber>([=](auto c) { attachTerrain(c); });
    CommandBus::subscribe(typeid(AttachTerrainToWorldMap), m_attachTerrain);

    m_onSceneGraphBuilt = std::make_shared<EventSubscriber>([=](auto e) { onSceneGraphBuilt(e); });
    EventPublisher::subscribe(typeid(FactorySceneGraphBuilt), m_onSceneGraphBuilt);
    m_onLazyNodeInstanced = std::make_shared<EventSubscriber>([=](auto e) { onLazyNodeInstanced(e); });
    EventPublisher::subscribe(typeid(LazyNodeInstanced), m_onLazyNodeInstanced);

    m_queryFittingNode = std::make_shared<QuerySubscriber>([=](const IQueryPtr& q) { queryFittingNode(q); });
    QueryDispatcher::subscribe(typeid(QueryFittingNode), m_queryFittingNode);

    return ServiceResult::Complete;
}

ServiceResult WorldMapService::onTerm()
{
    CommandBus::unsubscribe(typeid(CreateEmptyWorldMap), m_createWorldMap);
    m_createWorldMap = nullptr;
    CommandBus::unsubscribe(typeid(DeserializeWorldMap), m_deserializeWorldMap);
    m_deserializeWorldMap = nullptr;
    CommandBus::unsubscribe(typeid(AttachTerrainToWorldMap), m_attachTerrain);
    m_attachTerrain = nullptr;

    EventPublisher::unsubscribe(typeid(FactorySceneGraphBuilt), m_onSceneGraphBuilt);
    m_onSceneGraphBuilt = nullptr;
    EventPublisher::unsubscribe(typeid(LazyNodeInstanced), m_onLazyNodeInstanced);
    m_onLazyNodeInstanced = nullptr;

    QueryDispatcher::unsubscribe(typeid(QueryFittingNode), m_queryFittingNode);
    m_queryFittingNode = nullptr;

    return ServiceResult::Complete;
}

std::vector<Enigma::Engine::GenericDtoCollection> WorldMapService::serializeQuadNodeGraphs() const
{
    assert(!m_world.expired());

    std::vector<Engine::GenericDtoCollection> collections;
    EnumDerivedSpatials enumNode(VisibilityManagedNode::TYPE_RTTI);
    m_world.lock()->VisitBy(&enumNode);
    if (enumNode.GetSpatials().empty()) return collections;

    for (auto& node : enumNode.GetSpatials())
    {
        //todo : if quad node has portal node child, then need serialize portal node
        SceneFlattenTraversal flatten;
        node->VisitBy(&flatten);
        if (flatten.GetSpatials().empty()) continue;
        Engine::GenericDtoCollection collection;
        for (auto& sp : flatten.GetSpatials())
        {
            collection.push_back(sp->SerializeDto());
        }
        collection[0].AsTopLevel(true);
        collections.push_back(collection);
    }
    return collections;
}

Enigma::Engine::GenericDtoCollection WorldMapService::serializeWorldMap() const
{
    assert(!m_world.expired());
    Engine::GenericDtoCollection collection;
    auto world_dto = m_world.lock()->SerializeDto();
    world_dto.AsTopLevel(true);
    collection.push_back(world_dto);
    for (auto node : m_listQuadRoot)
    {
        if (node.expired()) continue;
        collection.push_back(node.lock()->SerializeAsLaziness());
    }
    for (const auto& child : m_world.lock()->GetChildList())
    {
        if (!child) continue;
        EnumNonDerivedSpatials enumSpatials(LazyNode::TYPE_RTTI);
        child->VisitBy(&enumSpatials);
        for (const auto& spatial : enumSpatials.GetSpatials())
        {
            if (!spatial) continue;
            collection.push_back(spatial->SerializeDto());
        }
    }
    return collection;
}

void WorldMapService::deserializeWorldMap(const Engine::GenericDtoCollection& graph)
{
    CommandBus::post(std::make_shared<BuildSceneGraph>(WORLD_MAP_TAG, graph));
}

void WorldMapService::attachTerrainToWorldMap(const std::shared_ptr<TerrainPawn>& terrain,
    const Matrix4& local_transform)
{
    assert(terrain);
    assert(!m_sceneGraphRepository.expired());
    if (FATAL_LOG_EXPR(m_world.expired())) return;
    std::string node_name = terrain->GetSpatialName() + QUADROOT_POSTFIX; // +NODE_FILE_EXT;
    auto quadRootNode = std::dynamic_pointer_cast<VisibilityManagedNode, Node>(m_sceneGraphRepository.lock()->CreateNode(node_name, VisibilityManagedNode::TYPE_RTTI));
    quadRootNode->TheLazyStatus().changeStatus(LazyStatus::Status::Ready);
    quadRootNode->TheFactoryDesc().ClaimAsInstanced(node_name + ".node");
    quadRootNode->AttachChild(terrain, Matrix4::IDENTITY);
    m_world.lock()->AttachChild(quadRootNode, local_transform);
    m_listQuadRoot.push_back(quadRootNode);
}

std::shared_ptr<Node> WorldMapService::queryFittingNode(const Engine::BoundingVolume& bv_in_world) const
{
    assert(!m_world.expired());
    if (m_listQuadRoot.empty()) return m_world.lock()->getPortalRootNode();
    for (auto root : m_listQuadRoot)
    {
        if (root.expired()) continue;
        Matrix4 root_inv_world_mx = root.lock()->GetWorldTransform().Inverse();
        auto bv_in_node = Engine::BoundingVolume::CreateFromTransform(bv_in_world, root_inv_world_mx);
        if (auto node = findFittingNodeFromQuadRoot(root.lock(), bv_in_node)) return node;
    }
    return m_world.lock()->getPortalRootNode();
}

std::shared_ptr<Node> WorldMapService::findFittingNodeFromQuadRoot(const std::shared_ptr<Node>& root, const Engine::BoundingVolume& bv_in_root) const
{
    auto root_local_box = root->GetModelBound().BoundingBox3();
    if (!root_local_box) return root;
    Vector3 root_box_min = root_local_box->Center() - Vector3(root_local_box->Extent());
    Vector3 root_box_max = root_local_box->Center() + Vector3(root_local_box->Extent());
    Vector3 local_pos = bv_in_root.Center();
    // if out of map bounding, return null
    if ((local_pos.X() < root_box_min.X())
        || (local_pos.X() > root_box_max.X())
        || (local_pos.Z() < root_box_min.Z())
        || (local_pos.Z() > root_box_max.Z())) return nullptr;

    return findFittingQuadLeaf(root, bv_in_root, MAX_RECURSIVE_DEPTH);
}

std::shared_ptr<Node> WorldMapService::findFittingQuadLeaf(const std::shared_ptr<SceneGraph::Node>& parent, const Engine::BoundingVolume& bv_in_node, int recursive_depth) const
{
    if (recursive_depth < 0) return parent;
    if (!parent) return nullptr;

    if (parent->GetChildList().empty()) return parent;
    const auto parent_node_box = parent->GetModelBound().BoundingBox3();
    if (!parent_node_box) return parent;

    Vector3 local_pos = bv_in_node.Center();
    auto [sub_tree_box_in_parent, sub_tree_index] = locateSubTreeBoxAndIndex(parent_node_box.value(), local_pos);
    bool envelop = testSubTreeQuadEnvelop(sub_tree_box_in_parent, bv_in_node);
    if (!envelop) return parent;  // 下一層放不下這物件，返回本層

    std::shared_ptr<Node> fitting_node = nullptr;
    std::string parent_node_unfix_name = parent->GetSpatialName();
    parent_node_unfix_name = parent_node_unfix_name.substr(0, parent_node_unfix_name.length()); // -strlen(NODE_FILE_EXT));
    std::string target_node_name = parent_node_unfix_name + "_" + string_format("%d", sub_tree_index); // +NODE_FILE_EXT;
    FindSpatialByName find_spatial(target_node_name);
    SceneTraveler::TravelResult result_find_node = parent->VisitBy(&find_spatial);
    if (result_find_node == SceneTraveler::TravelResult::InterruptTargetFound)
    {
        fitting_node = std::dynamic_pointer_cast<Node, Spatial>(find_spatial.GetFoundSpatial());
    }
    //todo : 原做法會再細切子節點，違背 CQS原則，要改成 query 失敗後，再送 command 細切子節點
    return fitting_node;
}

std::tuple<Box3, unsigned> WorldMapService::locateSubTreeBoxAndIndex(const MathLib::Box3& parent_box, const MathLib::Vector3& local_pos) const
{
    Vector3 sub_quad_center;
    unsigned leaf_index = 0;
    if (local_pos.X() > parent_box.Center().X())
    {
        leaf_index |= 0x01;
        sub_quad_center.X() = parent_box.Center().X() + parent_box.Extent()[0] / 2.0f;
    }
    else
    {
        sub_quad_center.X() = parent_box.Center().X() - parent_box.Extent()[0] / 2.0f;
    }
    if (local_pos.Z() > parent_box.Center().Z())
    {
        leaf_index |= 0x02;
        sub_quad_center.Z() = parent_box.Center().Z() + parent_box.Extent()[2] / 2.0f;
    }
    else
    {
        sub_quad_center.Z() = parent_box.Center().Z() - parent_box.Extent()[2] / 2.0f;
    }
    Box3 box_sub_tree;
    box_sub_tree.Center() = sub_quad_center;
    box_sub_tree.Extent()[0] = parent_box.Extent()[0] / 2.0f;
    box_sub_tree.Extent()[1] = parent_box.Extent()[1] / 2.0f;
    box_sub_tree.Extent()[2] = parent_box.Extent()[2] / 2.0f;
    return { box_sub_tree, leaf_index };
}

bool WorldMapService::testSubTreeQuadEnvelop(const MathLib::Box3& quad_box_in_parent, const Engine::BoundingVolume& bv_in_parent) const
{
    Box2 box_q(Vector2(quad_box_in_parent.Center().X(), quad_box_in_parent.Center().Z()),
        Vector2(quad_box_in_parent.Axis()[0].X(), quad_box_in_parent.Axis()[0].Z()),
        Vector2(quad_box_in_parent.Axis()[2].X(), quad_box_in_parent.Axis()[2].Z()), quad_box_in_parent.Extent()[0], quad_box_in_parent.Extent()[2]);
    if (auto bv_box = bv_in_parent.BoundingBox3()) // model bound is box
    {
        Box3 box_model = bv_box.value();
        Box2 box_m(Vector2(box_model.Center().X(), box_model.Center().Z()), Vector2::UNIT_X, Vector2::UNIT_Y,
            box_model.Extent()[0], box_model.Extent()[2]);
        return ContainmentBox2::TestBox2EnvelopBox2(box_q, box_m);
    }
    if (auto bv_sphere = bv_in_parent.BoundingSphere3())
    {
        Sphere3 sphere_model = bv_sphere.value();
        Sphere2 sphere_m(Vector2(sphere_model.Center().X(), sphere_model.Center().Z()), sphere_model.Radius());
        return ContainmentBox2::TestBox2EnvelopSphere2(box_q, sphere_m);
    }
    return false;
}

void WorldMapService::createEmptyWorldMap(const ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<CreateEmptyWorldMap, ICommand>(c);
    if (!cmd) return;
    std::vector<Engine::GenericDto> dtos = { cmd->getDto() };
    CommandBus::post(std::make_shared<BuildSceneGraph>(WORLD_MAP_TAG, dtos));
}

void WorldMapService::deserializeWorldMap(const ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<DeserializeWorldMap, ICommand>(c);
    if (!cmd) return;
    deserializeWorldMap(cmd->getGraph());
}

void WorldMapService::attachTerrain(const ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<AttachTerrainToWorldMap, ICommand>(c);
    if (!cmd) return;
    attachTerrainToWorldMap(cmd->getTerrain(), cmd->getLocalTransform());
}

void WorldMapService::onSceneGraphBuilt(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<FactorySceneGraphBuilt, IEvent>(e);
    if ((!ev) || (ev->GetSceneGraphId() != WORLD_MAP_TAG)) return;
    m_world = std::dynamic_pointer_cast<WorldMap, Spatial>(ev->GetTopLevelSpatial()[0]);
    if (!m_world.expired())
    {
        m_world.lock()->TheLazyStatus().changeStatus(LazyStatus::Status::Ready);  // empty world map is ready
        EventPublisher::post(std::make_shared<WorldMapCreated>(m_world.lock()->getName(), m_world.lock()));
        CommandBus::post(std::make_shared<AttachPortalOutsideZone>(m_world.lock()));
    }
}

void WorldMapService::onLazyNodeInstanced(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<LazyNodeInstanced, IEvent>(e);
    if ((!ev) || (!ev->GetNode())) return;
    if (ev->GetNode()->GetParent() == m_world.lock())
    {
        m_listQuadRoot.push_back(ev->GetNode());
    }
}

void WorldMapService::queryFittingNode(const IQueryPtr& q) const
{
    if (!q) return;
    const auto query = std::dynamic_pointer_cast<QueryFittingNode, IQuery>(q);
    if (!query) return;
    const auto node = queryFittingNode(query->getBoundingVolume());
    query->setResult(node);
}
