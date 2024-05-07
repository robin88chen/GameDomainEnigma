#include "QuadTreeRoot.h"
#include "QuadTreeRootDto.h"
#include "QuadTreeVolume.h"
#include "SceneGraph/SceneGraphQueries.h"
#include "WorldMapErrors.h"
#include "WorldMapEvents.h"
#include "Frameworks/EventPublisher.h"
#include "SceneGraph/SceneGraphAssemblers.h"
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

std::optional<SpatialId> QuadTreeRoot::findFittingNode(const Engine::BoundingVolume& bv_in_world) const
{
    auto root = std::make_shared<QuadTreeVolume>(m_rootNodeId);
    if (!root->isWorldPositionInside(bv_in_world.Center())) return std::nullopt;
    auto fitting_volume = root->findFittingVolume(bv_in_world);
    if (!fitting_volume) return std::nullopt;
    return fitting_volume->id();
}

void QuadTreeRoot::createFittingNode(const Engine::BoundingVolume& bv_in_world, unsigned max_depth)
{
    assert(!m_rootNodeId.empty());
    auto worldTransform = std::make_shared<QueryWorldTransform>(m_rootNodeId)->dispatch();
    auto modelBound = std::make_shared<QueryModelBound>(m_rootNodeId)->dispatch();
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
        bool has_node = std::make_shared<HasSpatial>(quad_tree_volume->id())->dispatch();
        if (has_node) continue;
        createTreeNode(quad_tree_volume);
    }
    EventPublisher::post(std::make_shared<FittingNodeCreated>(m_id));
}

void QuadTreeRoot::createTreeNode(const std::shared_ptr<QuadTreeVolume>& volume)
{

}

