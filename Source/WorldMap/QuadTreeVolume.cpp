#include "QuadTreeVolume.h"
#include "SceneGraph/SceneGraphQueries.h"

using namespace Enigma::WorldMap;
using namespace Enigma::SceneGraph;
using namespace Enigma::Engine;
using namespace Enigma::MathLib;

QuadTreeVolume::QuadTreeVolume(const SpatialId& id)
{
    m_id = id;
    if (m_id.isValid())
    {
        m_node = std::dynamic_pointer_cast<Node>(Node::queryNode(m_id));
    }
    if (m_node)
    {
        for (unsigned i = 0; i < maxChildren; ++i)
        {
            auto sub_volume_node_id = subVolumeNodeId(i);
            if (auto has_node = std::make_shared<HasSpatial>(sub_volume_node_id)->dispatch()) m_children[i] = std::make_shared<QuadTreeVolume>(sub_volume_node_id);
        }
    }
}

std::shared_ptr<QuadTreeVolume> QuadTreeVolume::findFittingVolume(const BoundingVolume& bv_in_world)
{
    if (!m_node) return nullptr;
    Matrix4 inv_world_mx = m_node->getWorldTransform().Inverse();
    auto bv_in_node = BoundingVolume::CreateFromTransform(bv_in_world, inv_world_mx);
    return nullptr;
}

SpatialId QuadTreeVolume::subVolumeNodeId(unsigned index)
{
    assert(index < maxChildren);
    auto sub_node_name = m_id.name() + "_" + std::to_string(index);
    return { sub_node_name, m_id.rtti() };
}

