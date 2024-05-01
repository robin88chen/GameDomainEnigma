#include "QuadTreeVolume.h"
#include "SceneGraph/SceneGraphQueries.h"

using namespace Enigma::WorldMap;
using namespace Enigma::SceneGraph;
using namespace Enigma::Engine;
using namespace Enigma::MathLib;

QuadTreeVolume::QuadTreeVolume(const SpatialId& id)
{
    m_id = id;
    if (!m_id.empty())
    {
        m_worldTransform = std::make_shared<QueryWorldTransform>(m_id)->dispatch();
        m_modelBounding = std::make_shared<QueryModelBound>(m_id)->dispatch();
        for (unsigned i = 0; i < maxChildren; ++i)
        {
            auto sub_volume_node_id = subVolumeNodeId(i);
            if (auto has_node = std::make_shared<HasSpatial>(sub_volume_node_id)->dispatch()) m_children[i] = std::make_shared<QuadTreeVolume>(sub_volume_node_id);
        }
    }
}

bool QuadTreeVolume::isWorldPositionInside(const MathLib::Vector3& worldPosition) const
{
    if (m_id.empty()) return false;
    auto local_position = m_worldTransform.Inverse() * worldPosition;
    return (m_modelBounding.PointInsideFlags(local_position) & static_cast<GenericBV::FlagBits>(GenericBV::TestedAxis::XZ)).all();
}

std::shared_ptr<QuadTreeVolume> QuadTreeVolume::findFittingVolume(const BoundingVolume& bv_in_world)
{
    if (m_id.empty()) return nullptr;
    Matrix4 inv_world_mx = m_worldTransform.Inverse();
    auto bv_in_node = BoundingVolume::CreateFromTransform(bv_in_world, inv_world_mx);
    return nullptr;
}

SpatialId QuadTreeVolume::subVolumeNodeId(unsigned index)
{
    assert(!m_id.empty());
    assert(index < maxChildren);
    auto sub_node_name = m_id.name() + "_" + std::to_string(index);
    return { sub_node_name, m_id.rtti() };
}

