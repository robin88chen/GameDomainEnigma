#include "WorldMapService.h"
#include "WorldMap.h"
#include "WorldMapErrors.h"
#include "Frameworks/CommandBus.h"
#include "Frameworks/EventPublisher.h"
#include "Frameworks/QueryDispatcher.h"
#include "WorldMapCommands.h"
#include "WorldMapDto.h"
#include "WorldMapEvents.h"
#include "WorldMapQueries.h"
#include "SceneGraph/SceneGraphCommands.h"
#include "SceneGraph/SceneGraphEvents.h"
#include "Frameworks/LazyStatus.h"
#include "Platforms/PlatformLayer.h"
#include "SceneGraph/PortalCommands.h"
#include "SceneGraph/VisibilityManagedNode.h"
#include "SceneGraph/EnumDerivedSpatials.h"
#include "SceneGraph/SceneFlattenTraversal.h"
#include "SceneGraph/Spatial.h"
#include "Frameworks/StringFormat.h"
#include "GameCommon/GameSceneCommands.h"
#include "SceneGraph/FindSpatialById.h"
#include "MathLib/Box2.h"
#include "MathLib/ContainmentBox2.h"
#include "SceneGraph/SceneGraphDtos.h"

using namespace Enigma::WorldMap;
using namespace Enigma::Frameworks;
using namespace Enigma::SceneGraph;
using namespace Enigma::Terrain;
using namespace Enigma::MathLib;

DEFINE_RTTI(WorldMap, WorldMapService, ISystemService);

std::string WORLD_MAP_TAG = "world_map";
std::string FITTING_NODE_TAG = "fitting_node";

constexpr int MAX_RECURSIVE_DEPTH = 4;

using error = std::error_code;

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
    m_createFittingNode = std::make_shared<CommandSubscriber>([=](const ICommandPtr& c) { createFittingNode(c); });
    CommandBus::subscribe(typeid(CreateFittingQuadNode), m_createFittingNode);

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
    CommandBus::unsubscribe(typeid(CreateFittingQuadNode), m_createFittingNode);
    m_createFittingNode = nullptr;

    EventPublisher::unsubscribe(typeid(FactorySceneGraphBuilt), m_onSceneGraphBuilt);
    m_onSceneGraphBuilt = nullptr;
    EventPublisher::unsubscribe(typeid(LazyNodeInstanced), m_onLazyNodeInstanced);
    m_onLazyNodeInstanced = nullptr;

    QueryDispatcher::unsubscribe(typeid(QueryFittingNode), m_queryFittingNode);
    m_queryFittingNode = nullptr;

    return ServiceResult::Complete;
}

std::vector<Enigma::Engine::GenericDtoCollection> WorldMapService::serializeWorldSceneGraphs() const
{
    assert(m_world);
    return m_world->serializeSceneGraphs();
}

Enigma::Engine::GenericDto WorldMapService::serializeWorldMap() const
{
    assert(m_world);
    return m_world->serializeDto();
}

std::shared_ptr<WorldMap> WorldMapService::deserializeWorldMap(const std::string& name, const Engine::GenericDtoCollection& graph, const std::shared_ptr<SceneGraph::PortalManagementNode>& portal_management_node)
{
    assert(!m_sceneGraphRepository.expired());
    assert(!graph.empty());
    return std::make_shared<WorldMap>(m_sceneGraphRepository.lock(), graph[0]);
    //WorldMapDto dto = WorldMapDto::fromGenericDto(graph[0]);
    //PortalZoneNodeDto portal_zone_dto = PortalZoneNodeDto::fromGenericDto(dto.portalRoot());
    //auto root_node = std::dynamic_pointer_cast<PortalZoneNode>(m_sceneGraphRepository.lock()->createNode(portal_zone_dto.name(), portal_zone_dto.factoryDesc()));
    //if (portal_management_node) portal_management_node->attachOutsideZone(root_node);
    //const auto world = std::make_shared<WorldMap>(name, dto.factoryDesc(), root_node);
    //return world;
}

std::shared_ptr<WorldMap> WorldMapService::createWorldMap(const std::string& name, const Engine::FactoryDesc& factory_desc, const std::shared_ptr<SceneGraph::PortalManagementNode>& portal_management_node)
{
    assert(!m_sceneGraphRepository.expired());
    auto root_node = std::dynamic_pointer_cast<PortalZoneNode>(m_sceneGraphRepository.lock()->createNode(name, Engine::FactoryDesc(PortalZoneNode::TYPE_RTTI.getName()).ClaimAsInstanced(name + ".node").PathId(factory_desc.PathId())));
    if (portal_management_node) portal_management_node->attachOutsideZone(root_node);
    const auto world = std::make_shared<WorldMap>(name, factory_desc, root_node);
    return world;
}

void WorldMapService::completeCreateWorldMap(const std::shared_ptr<WorldMap>& world)
{
    m_world = world;
    if (m_world)
    {
        EventPublisher::post(std::make_shared<WorldMapCreated>(m_world->getName(), m_world));
    }
}

void WorldMapService::completeDeserializeWorldMap(const std::shared_ptr<WorldMap>& world)
{
    m_world = world;
    if (m_world)
    {
        EventPublisher::post(std::make_shared<WorldMapDeserialized>(m_world->getName(), m_world));
    }
}

void WorldMapService::failCreateWorldMap(const std::string& name, std::error_code err)
{
    EventPublisher::post(std::make_shared<CreateWorldMapFailed>(name, err));
}

void WorldMapService::failDeserializeWorldMap(const std::string& name, std::error_code err)
{
    EventPublisher::post(std::make_shared<DeserializeWorldMapFailed>(name, err));
}

void WorldMapService::attachTerrainToWorldMap(const std::shared_ptr<TerrainPawn>& terrain,
    const Matrix4& local_transform)
{
    assert(m_world);
    assert(terrain);
    assert(!m_sceneGraphRepository.expired());
    m_world->attachTerrain(m_sceneGraphRepository.lock(), terrain, local_transform);
}

std::shared_ptr<Node> WorldMapService::queryFittingNode(const Engine::BoundingVolume& bv_in_world) const
{
    assert(m_world);
    return m_world->queryFittingNode(bv_in_world);
}

std::shared_ptr<Node> WorldMapService::findFittingNodeFromQuadRoot(const std::shared_ptr<Node>& root, const Engine::BoundingVolume& bv_in_root) const
{
    auto root_local_box = root->getModelBound().BoundingBox3();
    if (!root_local_box) return root;
    Vector3 root_box_min = root_local_box->Center() - Vector3(root_local_box->Extent());
    Vector3 root_box_max = root_local_box->Center() + Vector3(root_local_box->Extent());
    Vector3 local_pos = bv_in_root.Center();
    // if out of map bounding, return null
    if ((local_pos.x() < root_box_min.x())
        || (local_pos.x() > root_box_max.x())
        || (local_pos.z() < root_box_min.z())
        || (local_pos.z() > root_box_max.z())) return nullptr;

    return findFittingQuadLeaf(root, bv_in_root, MAX_RECURSIVE_DEPTH);
}

std::shared_ptr<Node> WorldMapService::findFittingQuadLeaf(const std::shared_ptr<SceneGraph::Node>& parent, const Engine::BoundingVolume& bv_in_node, int recursive_depth) const
{
    if (recursive_depth < 0) return parent;
    if (!parent) return nullptr;

    if (parent->getChildList().empty()) return parent;
    const auto parent_node_box = parent->getModelBound().BoundingBox3();
    if (!parent_node_box) return parent;

    Vector3 local_pos = bv_in_node.Center();
    auto [sub_tree_box_in_parent, sub_tree_index] = locateSubTreeBoxAndIndex(parent_node_box.value(), local_pos);
    bool envelop = testSubTreeQuadEnvelop(sub_tree_box_in_parent, bv_in_node);
    if (!envelop) return parent;  // 下一層放不下這物件，返回本層

    auto fitting_node = findTargetSubtree(parent, parent->getSpatialName(), sub_tree_index);
    if (!fitting_node) return nullptr;
    //todo : 原做法會再細切子節點，違背 CQS原則，要改成 query 失敗後，再送 command 細切子節點
    Matrix4 fitting_node_inv_local_mx = fitting_node->getLocalTransform().Inverse();
    auto bv_in_fitting_node = Engine::BoundingVolume::CreateFromTransform(bv_in_node, fitting_node_inv_local_mx);
    return findFittingQuadLeaf(fitting_node, bv_in_fitting_node, recursive_depth - 1);
}

void WorldMapService::createFittingNode(const Engine::BoundingVolume& bv_in_world)
{
    assert(m_world);
    if (m_listQuadRoot.empty()) return failCreateFittingNode(ErrorCode::emptyQuadRoot);
    error er;
    for (auto root : m_listQuadRoot)
    {
        if (root.expired()) continue;
        Matrix4 root_inv_world_mx = root.lock()->getWorldTransform().Inverse();
        auto bv_in_node = Engine::BoundingVolume::CreateFromTransform(bv_in_world, root_inv_world_mx);
        er = tryCreateFittingNodeFromQuadRoot(root.lock(), bv_in_node);
        if (er == ErrorCode::ok) return;
        if (er != ErrorCode::outOfBounds) return failCreateFittingNode(er);
    }
    failCreateFittingNode(er);
}

std::error_code WorldMapService::tryCreateFittingNodeFromQuadRoot(const std::shared_ptr<SceneGraph::Node>& root, const Engine::BoundingVolume& bv_in_root)
{
    auto root_local_box = root->getModelBound().BoundingBox3();
    if (!root_local_box) return ErrorCode::invalidBoundingBox;
    Vector3 root_box_min = root_local_box->Center() - Vector3(root_local_box->Extent());
    Vector3 root_box_max = root_local_box->Center() + Vector3(root_local_box->Extent());
    Vector3 local_pos = bv_in_root.Center();
    // if out of map bounding, return null
    if ((local_pos.x() < root_box_min.x())
        || (local_pos.x() > root_box_max.x())
        || (local_pos.z() < root_box_min.z())
        || (local_pos.z() > root_box_max.z())) return ErrorCode::outOfBounds;

    return tryCreateFittingQuadLeaves(root, bv_in_root);
}

std::error_code WorldMapService::tryCreateFittingQuadLeaves(const std::shared_ptr<SceneGraph::Node>& root, const Engine::BoundingVolume& bv_in_root)
{
    if (!root) return ErrorCode::nullQuadNode;
    auto root_node_box = root->getModelBound().BoundingBox3();
    if (!root_node_box) return ErrorCode::invalidBoundingBox;
    auto dtos = createFittingQuadGraph(root, bv_in_root);
    if (dtos.empty()) return ErrorCode::emptyQuadGraph;
    CommandBus::post(std::make_shared<BuildSceneGraph>(FITTING_NODE_TAG, dtos));
    return ErrorCode::ok;
}

Enigma::Engine::GenericDtoCollection WorldMapService::createFittingQuadGraph(const std::shared_ptr<Node>& root, const Engine::BoundingVolume& dest_bv_in_root)
{
    Engine::GenericDtoCollection dtos;
    std::vector<VisibilityManagedNodeDto> sub_quad_dtos;

    int depth = MAX_RECURSIVE_DEPTH;
    std::string parent_node_name = root->getSpatialName();
    //Matrix4 parent_world_mx = root->getWorldTransform();
    MathLib::Box3 parent_box = root->getModelBound().BoundingBox3().value();
    Engine::BoundingVolume dest_bv_in_node = dest_bv_in_root;
    auto dest_box = dest_bv_in_node.BoundingBox3();
    if (!dest_box) return dtos;
    bool has_sub_tree = true;

    while (depth >= 0)
    {
        Vector3 dest_local_pos = dest_box.value().Center();
        auto [sub_tree_box_in_parent, sub_tree_index] = locateSubTreeBoxAndIndex(parent_box, dest_local_pos);
        bool envelop = testSubTreeQuadEnvelop(sub_tree_box_in_parent, dest_bv_in_node);
        if (!envelop) break; // 下一層放不下這物件，結束
        std::shared_ptr<Node> sub_tree = nullptr;
        if (has_sub_tree)
        {
            sub_tree = findTargetSubtree(root, parent_node_name, sub_tree_index);
            if (sub_tree)
            {
                parent_node_name = sub_tree->getSpatialName();
                parent_box = sub_tree->getModelBound().BoundingBox3().value();
                Matrix4 parent_inv_local_mx = sub_tree->getLocalTransform().Inverse();
                dest_bv_in_node = Engine::BoundingVolume::CreateFromTransform(dest_bv_in_node, parent_inv_local_mx);
                dest_box = dest_bv_in_node.BoundingBox3();
                depth--;
            }
            else
            {
                has_sub_tree = false;
            }
        }
        if (!has_sub_tree)
        {
            VisibilityManagedNodeDto dto = createSubQuadNodeDto(parent_node_name, sub_tree_index, sub_tree_box_in_parent);
            if (sub_quad_dtos.empty())
            {
                dto.isTopLevel() = true;
                //m_fittingParentName = parent_node_name;
                m_fittingParentId = SpatialId(parent_node_name, VisibilityManagedNode::TYPE_RTTI);
            }
            linkQuadTreeChild(sub_quad_dtos, parent_node_name, dto.name());
            parent_node_name = dto.name();
            parent_box = Engine::BoundingVolumeDto::fromGenericDto(dto.modelBound()).box().value();
            Matrix4 parent_inv_local_mx = dto.localTransform().Inverse();
            dest_bv_in_node = Engine::BoundingVolume::CreateFromTransform(dest_bv_in_node, parent_inv_local_mx);
            dest_box = dest_bv_in_node.BoundingBox3();
            sub_quad_dtos.push_back(dto);
            depth--;
        }
    }
    if (sub_quad_dtos.empty()) return dtos;
    for (auto& dto : sub_quad_dtos)
    {
        dtos.push_back(dto.toGenericDto());
    }
    return dtos;
}

VisibilityManagedNodeDto WorldMapService::createSubQuadNodeDto(const std::string& parent_name, unsigned sub_tree_index, const MathLib::Box3& sub_tree_box_in_parent)
{
    VisibilityManagedNodeDto dto;
    dto.factoryDesc() = Engine::FactoryDesc(VisibilityManagedNode::TYPE_RTTI.getName());
    std::string target_node_name = parent_name + "_" + string_format("%d", sub_tree_index); // +NODE_FILE_EXT;
    dto.name() = target_node_name;
    Box3 sub_quad_box = sub_tree_box_in_parent;
    sub_quad_box.Center() = Vector3::ZERO;
    Engine::BoundingVolume sub_quad_bv = Engine::BoundingVolume{ sub_quad_box };
    Matrix4 local_transform = Matrix4::MakeTranslateTransform(sub_tree_box_in_parent.Center());
    dto.localTransform() = local_transform;
    dto.modelBound() = sub_quad_bv.serializeDto().toGenericDto();
    return dto;
}

void WorldMapService::linkQuadTreeChild(std::vector<SceneGraph::VisibilityManagedNodeDto>& node_dtos, const std::string& parent_name, const std::string& child_name)
{
    for (auto& dto : node_dtos)
    {
        if (dto.name() == parent_name)
        {
            //todo : use child id
            dto.childIds().push_back(SpatialId(child_name, VisibilityManagedNode::TYPE_RTTI));
            return;
        }
    }
}

std::tuple<Box3, unsigned> WorldMapService::locateSubTreeBoxAndIndex(const MathLib::Box3& parent_box, const MathLib::Vector3& local_pos) const
{
    Vector3 sub_quad_center;
    unsigned leaf_index = 0;
    if (local_pos.x() > parent_box.Center().x())
    {
        leaf_index |= 0x01;
        sub_quad_center.x() = parent_box.Center().x() + parent_box.Extent()[0] / 2.0f;
    }
    else
    {
        sub_quad_center.x() = parent_box.Center().x() - parent_box.Extent()[0] / 2.0f;
    }
    if (local_pos.z() > parent_box.Center().z())
    {
        leaf_index |= 0x02;
        sub_quad_center.z() = parent_box.Center().z() + parent_box.Extent()[2] / 2.0f;
    }
    else
    {
        sub_quad_center.z() = parent_box.Center().z() - parent_box.Extent()[2] / 2.0f;
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
    Box2 box_q(Vector2(quad_box_in_parent.Center().x(), quad_box_in_parent.Center().z()),
        Vector2(quad_box_in_parent.Axis()[0].x(), quad_box_in_parent.Axis()[0].z()),
        Vector2(quad_box_in_parent.Axis()[2].x(), quad_box_in_parent.Axis()[2].z()), quad_box_in_parent.Extent()[0], quad_box_in_parent.Extent()[2]);
    if (auto bv_box = bv_in_parent.BoundingBox3()) // model bound is box
    {
        Box3 box_model = bv_box.value();
        Box2 box_m(Vector2(box_model.Center().x(), box_model.Center().z()), Vector2::UNIT_X, Vector2::UNIT_Y,
            box_model.Extent()[0], box_model.Extent()[2]);
        return ContainmentBox2::TestBox2EnvelopBox2(box_q, box_m);
    }
    if (auto bv_sphere = bv_in_parent.BoundingSphere3())
    {
        Sphere3 sphere_model = bv_sphere.value();
        Sphere2 sphere_m(Vector2(sphere_model.Center().x(), sphere_model.Center().z()), sphere_model.Radius());
        return ContainmentBox2::TestBox2EnvelopSphere2(box_q, sphere_m);
    }
    return false;
}

/*std::error_code WorldMapService::tryCreateFittingQuadLeaf(const std::shared_ptr<SceneGraph::Node>& parent, const Engine::BoundingVolume& bv_in_node, int recursive_depth)
{
    if (recursive_depth < 0)
    {
        completeCreateFittingNode(parent);
        return ErrorCode::ok; // enough deep, completed
    }
    if (!parent) return ErrorCode::nullQuadNode;

    const auto parent_node_box = parent->getModelBound().BoundingBox3();
    if (!parent_node_box) return ErrorCode::invalidBoundingBox;

    Vector3 local_pos = bv_in_node.Center();
    auto [sub_tree_box_in_parent, sub_tree_index] = locateSubTreeBoxAndIndex(parent_node_box.value(), local_pos);
    bool envelop = testSubTreeQuadEnvelop(sub_tree_box_in_parent, bv_in_node);
    if (!envelop)
    {
        completeCreateFittingNode(parent);
        return ErrorCode::ok;  // 下一層放不下這物件，結束
    }
    auto fitting_node = findTargetSubtree(parent, parent->getSpatialName(), sub_tree_index);
    if (!fitting_node)
    {
        if (m_sceneGraphRepository.expired()) return ErrorCode::nullSceneGraphRepository;
        auto sub_quad_node = std::dynamic_pointer_cast<LazyNode>(m_sceneGraphRepository.lock()->CreateNode(target_node_name, VisibilityManagedNode::TYPE_RTTI));
        sub_quad_node->lazyStatus().changeStatus(LazyStatus::Status::Ready);
        sub_quad_node->factoryDesc().ClaimAsDeferred();
        Box3 sub_quad_box = sub_tree_box_in_parent;
        sub_quad_box.Center() = Vector3::ZERO;
        Engine::BoundingVolume sub_quad_bv = Engine::BoundingVolume{ sub_quad_box };
        Matrix4 mxLocal = Matrix4::MakeTranslateTransform(sub_tree_box_in_parent.Center());
        parent->attachChild(sub_quad_node, mxLocal);
        fitting_node = sub_quad_node;
    }
        Matrix4 fitting_node_inv_local_mx = fitting_node->getLocalTransform().Inverse();
        auto bv_in_fitting_node = Engine::BoundingVolume::CreateFromTransform(bv_in_node, fitting_node_inv_local_mx);
        return tryCreateFittingQuadLeaf(fitting_node, bv_in_fitting_node, recursive_depth - 1);
}*/

std::shared_ptr<Node> WorldMapService::findTargetSubtree(const std::shared_ptr<SceneGraph::Node>& any_level_parent, const std::string& parent_name, unsigned sub_tree_index) const
{
    std::shared_ptr<Node> found_node = nullptr;
    std::string target_node_name = parent_name + "_" + string_format("%d", sub_tree_index); // +NODE_FILE_EXT;
    //todo : find spatial by id
    auto target_node_id = SpatialId(target_node_name, VisibilityManagedNode::TYPE_RTTI);
    FindSpatialById find_spatial(target_node_id);
    SceneTraveler::TravelResult result_find_node = any_level_parent->visitBy(&find_spatial);
    if (result_find_node == SceneTraveler::TravelResult::InterruptTargetFound)
    {
        found_node = std::dynamic_pointer_cast<Node, Spatial>(find_spatial.getFoundSpatial());
    }
    return found_node;
}

void WorldMapService::completeCreateFittingNode(const std::shared_ptr<SceneGraph::Node>& node)
{
    CommandBus::post(std::make_shared<GameCommon::AttachNodeChild>(m_fittingParentId, node, node->getLocalTransform()));
    //CommandBus::post(std::make_shared<GameCommon::AttachNodeChild>(m_fittingParentName, node, node->getLocalTransform()));
    EventPublisher::post(std::make_shared<FittingNodeCreated>(m_createFittingNodeRuid, node));
}

void WorldMapService::failCreateFittingNode(std::error_code err)
{
    EventPublisher::post(std::make_shared<CreateFittingNodeFailed>(m_createFittingNodeRuid, err));
}

void WorldMapService::createEmptyWorldMap(const ICommandPtr& c)
{
    assert(!m_sceneGraphRepository.expired());
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<CreateEmptyWorldMap, ICommand>(c);
    if (!cmd) return;
    const auto portal_management_node = std::dynamic_pointer_cast<PortalManagementNode>(m_sceneGraphRepository.lock()->queryNode(cmd->portalManagerName()));
    if (!portal_management_node)
    {
        failCreateWorldMap(cmd->name(), ErrorCode::nullPortalManager);
        return;
    }
    const auto world = createWorldMap(cmd->name(), cmd->factoryDesc(), portal_management_node);
    completeCreateWorldMap(world);
}

void WorldMapService::deserializeWorldMap(const ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<DeserializeWorldMap, ICommand>(c);
    if (!cmd) return;
    const auto portal_management_node = std::dynamic_pointer_cast<PortalManagementNode>(m_sceneGraphRepository.lock()->queryNode(cmd->portalManagerName()));
    if (!portal_management_node)
    {
        failDeserializeWorldMap(cmd->name(), ErrorCode::nullPortalManager);
        return;
    }
    const auto world = deserializeWorldMap(cmd->name(), cmd->graph(), portal_management_node);
    completeDeserializeWorldMap(world);
}

void WorldMapService::attachTerrain(const ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<AttachTerrainToWorldMap, ICommand>(c);
    if (!cmd) return;
    attachTerrainToWorldMap(cmd->getTerrain(), cmd->getLocalTransform());
}

void WorldMapService::createFittingNode(const Frameworks::ICommandPtr& c)
{
    if (!c) return;
    const auto cmd = std::dynamic_pointer_cast<CreateFittingQuadNode, ICommand>(c);
    if (!cmd) return;
    m_createFittingNodeRuid = cmd->getRuid();
    createFittingNode(cmd->getBoundingVolume());
}

void WorldMapService::onSceneGraphBuilt(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<FactorySceneGraphBuilt, IEvent>(e);
    if (!ev) return;
    if (ev->GetSceneGraphId() == WORLD_MAP_TAG)
    {
        completeCreateWorldMap(std::dynamic_pointer_cast<WorldMap, Spatial>(ev->GetTopLevelSpatial()[0]));
    }
    else if (ev->GetSceneGraphId() == FITTING_NODE_TAG)
    {
        completeCreateFittingNode(std::dynamic_pointer_cast<Node, Spatial>(ev->GetTopLevelSpatial()[0]));
    }
}

void WorldMapService::onLazyNodeInstanced(const IEventPtr& e)
{
    if (!e) return;
    const auto ev = std::dynamic_pointer_cast<LazyNodeInstanced, IEvent>(e);
    if ((!ev) || (!ev->GetNode())) return;
    /*if (ev->GetNode()->getParent() == m_world)
    {
        m_listQuadRoot.push_back(ev->GetNode());
    }*/
}

void WorldMapService::queryFittingNode(const IQueryPtr& q) const
{
    if (!q) return;
    const auto query = std::dynamic_pointer_cast<QueryFittingNode, IQuery>(q);
    if (!query) return;
    const auto node = queryFittingNode(query->getBoundingVolume());
    query->setResult(node);
}
