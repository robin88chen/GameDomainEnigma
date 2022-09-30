#include "Spatial.h"
#include "Node.h"

using namespace Enigma::SceneGraph;

Spatial::Spatial(const std::string& name)
{
    m_name = name;

    m_graphDepth = 0;

    //m_spatialRenderState = nullptr;

    m_cullingMode = CullingMode::Dynamic;

    m_spatialFlags = SpatialBit::Spatial_BelongToParent | SpatialBit::Spatial_Unlit; // 預設是不受光的...entity再打開

    m_vecLocalPosition = MathLib::Vector3::ZERO;
    m_vecLocalScale = MathLib::Vector3(1.0f, 1.0f, 1.0f);
    m_mxLocalRotation = MathLib::Matrix3::IDENTITY;
    m_qtLocalQuaternion = MathLib::Quaternion::IDENTITY;
    m_vecLocalEulerAngle = MathLib::Vector3::ZERO;

    m_mxLocalTransform = MathLib::Matrix4::IDENTITY;
    m_mxWorldTransform = MathLib::Matrix4::IDENTITY;

    m_vecWorldPosition = MathLib::Vector3::ZERO;

    m_notifyFlags = NotifyBit::Notify_None;
}

Spatial::~Spatial()
{
    //m_spatialRenderState = nullptr;
}

void Spatial::LinkParent(const std::shared_ptr<Spatial>& parent)
{
    m_parent = parent;
    if ((!m_parent.expired()) && (m_parent.lock()))
        m_graphDepth = m_parent.lock()->GetGraphDepth() + 1;
}

std::shared_ptr<Spatial> Spatial::GetParent() const
{
    if (!m_parent.expired()) return m_parent.lock();
    return nullptr;
}

void Spatial::DetachFromParent()
{
    if (m_parent.expired()) return;
    const NodePtr parent_node = std::dynamic_pointer_cast<Node, Spatial>(m_parent.lock());
    if (!parent_node) return;
    parent_node->DetachChild(ThisSpatial());
}
