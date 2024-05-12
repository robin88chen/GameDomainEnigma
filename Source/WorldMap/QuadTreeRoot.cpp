#include "QuadTreeRoot.h"
#include "QuadTreeRootDto.h"
#include "QuadTreeVolume.h"
#include "WorldMapErrors.h"
#include "WorldMapEvents.h"
#include "WorldMapQueries.h"
#include "Frameworks/EventPublisher.h"
#include "SceneGraph/SceneGraphAssemblers.h"
#include "SceneGraph/SceneGraphFactory.h"
#include "SceneGraph/VisibilityManagedNode.h"
#include <cassert>

using namespace Enigma::WorldMap;
using namespace Enigma::SceneGraph;
using namespace Enigma::Frameworks;

QuadTreeRoot::QuadTreeRoot(const QuadTreeRootId& id) : m_id(id)
{
}

QuadTreeRoot::QuadTreeRoot(const QuadTreeRootId& id, const Engine::GenericDto& o) : m_id(id)
{
    QuadTreeRootDto dto(o);
    assert(dto.id() == m_id);
    m_rootNodeId = dto.rootNodeId();
    if (!m_rootNodeId.empty())
    {
        m_root = std::dynamic_pointer_cast<LazyNode>(Node::queryNode(m_rootNodeId));
    }
}

std::shared_ptr<QuadTreeRoot> QuadTreeRoot::queryQuadTreeRoot(const QuadTreeRootId& id)
{
    return std::make_shared<QueryQuadTreeRoot>(id)->dispatch();
}

std::optional<SpatialId> QuadTreeRoot::findFittingNode(const Engine::BoundingVolume& bv_in_world) const
{
    auto root = std::make_shared<QuadTreeVolume>(m_rootNodeId);
    if (!root->isWorldPositionInside(bv_in_world.Center())) return std::nullopt;
    auto fitting_volume = root->findFittingVolume(bv_in_world);
    if (!fitting_volume) return std::nullopt;
    return fitting_volume->id();
}

void QuadTreeRoot::createFittingNodes(const std::shared_ptr<SceneGraph::SceneGraphRepository>& repository, const Engine::BoundingVolume& bv_in_world, unsigned max_depth)
{
    assert(repository);
    assert(!m_rootNodeId.empty());
    auto worldTransform = repository->queryWorldTransform(m_rootNodeId);
    auto modelBound = repository->queryModelBound(m_rootNodeId);
    auto root = std::make_shared<QuadTreeVolume>(m_rootNodeId, std::nullopt, worldTransform, modelBound);
    root->constituteFullQuadTree(max_depth);
    auto fitting_volumes = root->collectFitVolumes(bv_in_world);
    if (fitting_volumes.empty())
    {
        EventPublisher::post(std::make_shared<FittingNodeCreationFailed>(m_id, ErrorCode::emptyQuadVolumeList));
        return;
    }
    for (const auto& quad_tree_volume : fitting_volumes)
    {
        if (!quad_tree_volume) return;
        if (repository->hasSpatial(quad_tree_volume->id())) continue;
        if (auto er = createTreeNode(repository, quad_tree_volume))
        {
            EventPublisher::post(std::make_shared<FittingNodeCreationFailed>(m_id, er));
            return;
        }
    }
    EventPublisher::post(std::make_shared<FittingNodeCreated>(m_id));
}

std::error_code QuadTreeRoot::createTreeNode(const std::shared_ptr<SceneGraph::SceneGraphRepository>& repository, const std::shared_ptr<QuadTreeVolume>& volume)
{
    assert(repository);
    assert(volume);
    if (!volume->parentId().has_value()) return ErrorCode::parentQuadNodeNotFound;
    std::shared_ptr<LazyNode> parent_node = std::dynamic_pointer_cast<LazyNode>(repository->querySpatial(volume->parentId().value()));
    if (!parent_node) return ErrorCode::parentQuadNodeNotFound;
    if (!parent_node->lazyStatus().isReady())
    {
        parent_node->lazyStatus().changeStatus(LazyStatus::Status::InQueue);
        repository->hydrateLazyNode(parent_node->id());
        if (!parent_node->lazyStatus().isReady()) return ErrorCode::parentQuadNodeHydrationFailed;
    }
    MathLib::Matrix4 local_transform = volume->worldTransform() * parent_node->getWorldTransform().Inverse();
    Engine::GenericDto child_dto = assembleChildTreeNode(volume->parentId().value(), parent_node->factoryDesc(), volume->id(), local_transform, volume->worldTransform(), volume->modelBounding());
    std::shared_ptr<LazyNode> child_node = std::dynamic_pointer_cast<LazyNode>(repository->factory()->constituteSpatial(volume->id(), child_dto, true));
    if (!child_node) return ErrorCode::childQuadNodeConstitutionFailed;
    std::error_code er = child_node->hydrate(child_dto);
    if (er) return er;
    repository->putSpatial(child_node, PersistenceLevel::Store);
    repository->putLaziedContent(child_node);
    er = parent_node->attachChild(child_node, local_transform);
    if (er) return er;
    repository->putSpatial(parent_node, PersistenceLevel::Store);
    repository->putLaziedContent(parent_node);
    return ErrorCode::ok;
}

Enigma::Engine::GenericDto QuadTreeRoot::assembleChildTreeNode(const SceneGraph::SpatialId& parent_id, const Engine::FactoryDesc& parent_desc, const SceneGraph::SpatialId& id, const MathLib::Matrix4& local_transform, const MathLib::Matrix4& world_transform, const Engine::BoundingVolume& model_bound)
{
    auto child_filename = replaceToChildFilename(parent_desc.GetDeferredFilename(), parent_desc.PathId(), parent_id, id);
    VisibilityManagedNodeAssembler assembler(id);
    assembler.lazyNode().node().spatial().localTransform(local_transform).modelBound(model_bound).worldTransform(world_transform);
    assembler.asDeferred(child_filename, parent_desc.PathId());
    return assembler.toHydratedGenericDto();
}

std::string QuadTreeRoot::replaceToChildFilename(const std::string& parent_filename, const std::string& path_id, const SceneGraph::SpatialId& parent_id, const SceneGraph::SpatialId& id)
{
    std::string child_filename = parent_filename;
    auto pos = child_filename.find_last_of('@');
    if (pos != std::string::npos)
    {
        child_filename = child_filename.substr(0, pos);
    }
    pos = child_filename.find(parent_id.name());
    assert(pos != std::string::npos);
    child_filename.replace(pos, parent_id.name().length(), id.name());
    return child_filename;
}

