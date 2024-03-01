#include "SceneQuadTreeRoot.h"
#include "QuadTreeRootDto.h"
#include "SceneGraph/FindSpatialById.h"
#include "SceneGraph/EnumDerivedSpatials.h"
#include "SceneGraph/VisibilityManagedNode.h"
#include "MathLib/Matrix4.h"
#include "Frameworks/StringFormat.h"
#include "MathLib/ContainmentBox2.h"
#include "Frameworks/EventPublisher.h"
#include "SceneGraph/SceneGraphEvents.h"

using namespace Enigma::WorldMap;
using namespace Enigma::SceneGraph;
using namespace Enigma::MathLib;
using namespace Enigma::Engine;

DEFINE_RTTI_OF_BASE(WorldMap, SceneQuadTreeRoot);

constexpr int MAX_RECURSIVE_DEPTH = 4;

SceneQuadTreeRoot::SceneQuadTreeRoot(const std::string& name, const std::shared_ptr<LazyNode>& root)
    : m_factory_desc(TYPE_RTTI.getName()), m_name(name), m_root(root)
{
}

SceneQuadTreeRoot::SceneQuadTreeRoot(const std::shared_ptr<SceneGraph::SceneGraphRepository>& repository, const Engine::GenericDto& o) : m_factory_desc(TYPE_RTTI.getName())
{
    QuadTreeRootDto dto = QuadTreeRootDto::fromGenericDto(o);
    m_name = dto.name();
    //todo: workaround for serialization
    if (repository->hasNode(dto.root().getName()))
    {
        m_root = std::dynamic_pointer_cast<LazyNode>(repository->queryNode(dto.root().getName()));
        assert(!m_root.expired());
    }
    else
    {
        m_workaround_root_dto = dto.root();
        m_workaround_on_factory_spatial_created = std::make_shared<Frameworks::EventSubscriber>([=](auto e) { workaround_onFactorySpatialCreated(e); });
        Frameworks::EventPublisher::subscribe(typeid(FactorySpatialCreated), m_workaround_on_factory_spatial_created);
    }
    //m_root = std::dynamic_pointer_cast<LazyNode>(repository->createNode(dto.root()));
    //assert(!m_root.expired());
}

SceneQuadTreeRoot::~SceneQuadTreeRoot()
{
    if (m_workaround_on_factory_spatial_created)
    {
        Frameworks::EventPublisher::unsubscribe(typeid(FactorySpatialCreated), m_workaround_on_factory_spatial_created);
        m_workaround_on_factory_spatial_created = nullptr;
    }
}

GenericDto SceneQuadTreeRoot::serializeDto()
{
    QuadTreeRootDto dto;
    dto.factoryDesc() = m_factory_desc;
    dto.name() = m_name;
    assert(!m_root.expired());
    dto.root() = m_root.lock()->serializeAsLaziness();
    return dto.toGenericDto();
}

std::vector<GenericDtoCollection> SceneQuadTreeRoot::serializeTreeGraphs()
{
    assert(!m_root.expired());
    std::vector<Engine::GenericDtoCollection> collections;
    EnumDerivedSpatials enumNode(VisibilityManagedNode::TYPE_RTTI);
    m_root.lock()->visitBy(&enumNode);
    if (enumNode.GetSpatials().empty()) return collections;

    for (auto& spatial : enumNode.GetSpatials())
    {
        const auto node = std::dynamic_pointer_cast<Node, Spatial>(spatial);
        if (!node) continue;
        //todo : if quad node has portal node child, then need serialize portal node
        collections.emplace_back(node->serializeFlattenedTree());
    }
    return collections;
}

std::shared_ptr<Node> SceneQuadTreeRoot::queryFittingNode(const BoundingVolume& bv_in_world) const
{
    assert(!m_root.expired());
    Matrix4 root_inv_world_mx = m_root.lock()->getWorldTransform().Inverse();
    auto bv_in_node = BoundingVolume::CreateFromTransform(bv_in_world, root_inv_world_mx);
    return findFittingNodeFromRoot(bv_in_node);
}

std::shared_ptr<Node> SceneQuadTreeRoot::findFittingNodeFromRoot(const Engine::BoundingVolume& bv_in_root) const
{
    assert(!m_root.expired());
    auto root_local_box = m_root.lock()->getModelBound().BoundingBox3();
    if (!root_local_box) return m_root.lock();
    Vector3 root_box_min = root_local_box->Center() - Vector3(root_local_box->Extent());
    Vector3 root_box_max = root_local_box->Center() + Vector3(root_local_box->Extent());
    Vector3 local_pos = bv_in_root.Center();
    // if out of map bounding, return null
    if ((local_pos.x() < root_box_min.x())
        || (local_pos.x() > root_box_max.x())
        || (local_pos.z() < root_box_min.z())
        || (local_pos.z() > root_box_max.z())) return nullptr;

    return findFittingLeaf(bv_in_root);
}

std::shared_ptr<Node> SceneQuadTreeRoot::findFittingLeaf(const Engine::BoundingVolume& bv_in_root) const
{
    assert(!m_root.expired());
    std::shared_ptr<Node> fitting_leaf = nullptr;
    int depth = MAX_RECURSIVE_DEPTH;
    std::shared_ptr<Node> parent = m_root.lock();
    auto bv_in_node = bv_in_root;
    while (depth >= 0)
    {
        if (!parent) return nullptr;
        if (parent->getChildList().empty()) return parent;
        const auto parent_node_box = parent->getModelBound().BoundingBox3();
        if (!parent_node_box) return parent;

        Vector3 local_pos = bv_in_node.Center();
        auto [sub_tree_box_in_parent, sub_tree_index] = locateSubTreeBoxAndIndex(parent_node_box.value(), local_pos);
        bool envelop = testSubTreeQuadEnvelop(sub_tree_box_in_parent, bv_in_root);
        if (!envelop) return parent;  // 下一層放不下這物件，返回本層

        fitting_leaf = findTargetSubtree(parent, parent->getSpatialName(), sub_tree_index);
        if (!fitting_leaf) return nullptr; // 找不到子節點，搜尋失敗

        parent = fitting_leaf;
        Matrix4 fitting_leaf_inv_local_mx = fitting_leaf->getLocalTransform().Inverse();
        auto bv_in_fitting_node = Engine::BoundingVolume::CreateFromTransform(bv_in_node, fitting_leaf_inv_local_mx);
        depth--;
    }
    return fitting_leaf;
}

std::tuple<Box3, unsigned> SceneQuadTreeRoot::locateSubTreeBoxAndIndex(const MathLib::Box3& parent_box, const MathLib::Vector3& local_pos) const
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

bool SceneQuadTreeRoot::testSubTreeQuadEnvelop(const MathLib::Box3& quad_box_in_parent, const Engine::BoundingVolume& bv_in_parent) const
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

std::shared_ptr<Node> SceneQuadTreeRoot::findTargetSubtree(const std::shared_ptr<SceneGraph::Node>& any_level_parent, const std::string& parent_name, unsigned sub_tree_index) const
{
    std::shared_ptr<Node> found_node = nullptr;
    std::string target_node_name = parent_name + "_" + string_format("%d", sub_tree_index); // +NODE_FILE_EXT;
    auto target_node_id = SpatialId(target_node_name, VisibilityManagedNode::TYPE_RTTI);
    //todo: find node by id
    FindSpatialById find_spatial(target_node_id);
    SceneTraveler::TravelResult result_find_node = any_level_parent->visitBy(&find_spatial);
    if (result_find_node == SceneTraveler::TravelResult::InterruptTargetFound)
    {
        found_node = std::dynamic_pointer_cast<Node, Spatial>(find_spatial.getFoundSpatial());
    }
    return found_node;
}

void SceneQuadTreeRoot::workaround_onFactorySpatialCreated(const Frameworks::IEventPtr& e)
{
    if (!e) return;
    auto ev = std::dynamic_pointer_cast<FactorySpatialCreated>(e);
    if (!ev) return;
    if (ev->GetDto().getName() == m_workaround_root_dto.getName())
    {
        m_root = std::dynamic_pointer_cast<LazyNode>(ev->GetSpatial());
        assert(!m_root.expired());
    }
}
