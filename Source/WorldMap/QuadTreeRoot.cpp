#include "QuadTreeRoot.h"
#include "QuadTreeRootDto.h"
#include "QuadTreeVolume.h"
#include <cassert>

using namespace Enigma::WorldMap;
using namespace Enigma::SceneGraph;

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

