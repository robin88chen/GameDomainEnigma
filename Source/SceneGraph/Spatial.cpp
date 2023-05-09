#include "Spatial.h"
#include "Node.h"
#include "SceneGraphErrors.h"
#include "Culler.h"
#include "SceneGraphEvents.h"
#include "SceneGraphDtos.h"
#include "MathLib/MathAlgorithm.h"
#include "Frameworks/EventPublisher.h"
#include "GameEngine/BoundingVolume.h"
#include <cassert>
#include <tuple>

using namespace Enigma::SceneGraph;
using namespace Enigma::MathLib;
using namespace Enigma::Engine;

DEFINE_RTTI_OF_BASE(SceneGraph, Spatial);

Spatial::Spatial(const std::string& name) : m_factoryDesc(Spatial::TYPE_RTTI.GetName())
{
    m_name = name;

    m_graphDepth = 0;

    //m_spatialRenderState = nullptr;

    m_cullingMode = CullingMode::Dynamic;

    m_spatialFlags = Spatial_BelongToParent | Spatial_Unlit; // 預設是不受光的...entity再打開

    m_vecLocalPosition = Vector3::ZERO;
    m_vecLocalScale = Vector3(1.0f, 1.0f, 1.0f);
    m_mxLocalRotation = Matrix3::IDENTITY;
    m_qtLocalQuaternion = Quaternion::IDENTITY;
    m_vecLocalEulerAngle = Vector3::ZERO;

    m_mxLocalTransform = Matrix4::IDENTITY;
    m_mxWorldTransform = Matrix4::IDENTITY;

    m_modelBound = BoundingVolume(Box3::UNIT_BOX);
    m_worldBound = BoundingVolume(Box3::UNIT_BOX);

    m_vecWorldPosition = Vector3::ZERO;

    m_notifyFlags = Notify_None;
}

Spatial::Spatial(const GenericDto& o) : m_factoryDesc(o.GetRtti())
{
    SpatialDto dto = SpatialDto::FromGenericDto(o);
    m_name = dto.Name();
    m_graphDepth = dto.GraphDepth();
    m_cullingMode = static_cast<CullingMode>(dto.CullingMode());
    m_spatialFlags = dto.SpatialFlag();
    m_notifyFlags = dto.NotifyFlag();
    m_mxLocalTransform = dto.LocalTransform();
    assert(m_mxLocalTransform != Matrix4::ZERO);
    m_mxWorldTransform = dto.WorldTransform();
    assert(m_mxWorldTransform != Matrix4::ZERO);
    m_modelBound = BoundingVolume(BoundingVolumeDto::FromGenericDto(dto.ModelBound()));
    m_worldBound = BoundingVolume(BoundingVolumeDto::FromGenericDto(dto.WorldBound()));
    assert(!m_modelBound.IsEmpty());
    assert(!m_worldBound.IsEmpty());
    std::tie(m_vecLocalScale, m_qtLocalQuaternion, m_vecLocalPosition) = m_mxLocalTransform.UnMatrixSRT();
    m_mxLocalRotation = m_qtLocalQuaternion.ToRotationMatrix();
    EulerAngles angles{ 0.0f, 0.0f, 0.0f };
    std::tie(angles, std::ignore) = m_mxLocalRotation.ToEulerAnglesXYZ();
    m_vecLocalEulerAngle = Vector3(angles.m_x, angles.m_y, angles.m_z);
    m_vecWorldPosition = m_mxWorldTransform.UnMatrixTranslate();
}

Spatial::~Spatial()
{
    //m_spatialRenderState = nullptr;
}

Enigma::Engine::GenericDto Spatial::SerializeDto()
{
    return SerializeSpatialDto().ToGenericDto();
}

SpatialDto Spatial::SerializeSpatialDto()
{
    SpatialDto dto;
    dto.TheFactoryDesc() = m_factoryDesc;
    dto.Name() = m_name;
    dto.GraphDepth() = m_graphDepth;
    dto.CullingMode() = static_cast<unsigned int>(m_cullingMode);
    dto.SpatialFlag() = static_cast<unsigned int>(m_spatialFlags.to_ulong());
    dto.NotifyFlag() = static_cast<unsigned int>(m_notifyFlags.to_ulong());
    dto.LocalTransform() = m_mxLocalTransform;
    dto.WorldTransform() = m_mxWorldTransform;
    dto.ModelBound() = m_modelBound.SerializeDto().ToGenericDto();
    dto.WorldBound() = m_worldBound.SerializeDto().ToGenericDto();
    return dto;
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

error Spatial::CullVisibleSet(Culler* culler, bool noCull)
{
    assert(culler);
    if (!CanVisited()) return ErrorCode::dataNotReady;
    if (m_cullingMode == CullingMode::Always) return ErrorCode::ok;
    if (TestSpatialFlag(Spatial_Hide)) return ErrorCode::ok;

    if (m_cullingMode == CullingMode::Never) noCull = true;

    error er = ErrorCode::ok;
    auto save_plane_activations = culler->GetPlaneActivations();
    if ((noCull) || (culler->IsVisible(m_worldBound)))
    {
        er = OnCullingVisible(culler, noCull);
    }
    else if (culler->IsOutVisibility(m_worldBound))
    {
        OnCullingCompleteNotVisible(culler);
    }

    culler->RestorePlaneBitFlags(save_plane_activations);

    return er;
}

Matrix4 Spatial::GetParentWorldTransform() const
{
    if (!m_parent.expired()) return m_parent.lock()->GetWorldTransform();
    return Matrix4::IDENTITY;
}

error Spatial::SetLocalPosition(const Vector3& pos)
{
    m_vecLocalPosition = pos;
    m_mxLocalTransform.SetColumn(3, Vector4(pos.X(), pos.Y(), pos.Z(), 1.0f));
    return _UpdateLocalTransform(m_mxLocalTransform);
}

error Spatial::SetLocalRotation(const Matrix3& rot)
{
    m_mxLocalRotation = rot;
    EulerAngles angles{ 0.0f, 0.0f, 0.0f };
    std::tie(angles, std::ignore) = m_mxLocalRotation.ToEulerAnglesXYZ();
    m_vecLocalEulerAngle = Vector3(angles.m_x, angles.m_y, angles.m_z);
    m_qtLocalQuaternion = Quaternion::FromRotationMatrix(m_mxLocalRotation);
    m_mxLocalTransform = Matrix4::FromSRT(m_vecLocalScale, m_qtLocalQuaternion, m_vecLocalPosition);
    return _UpdateLocalTransform(m_mxLocalTransform);
}

error Spatial::SetLocalRotation(const Quaternion& qt)
{
    m_qtLocalQuaternion = qt;
    m_mxLocalRotation = m_qtLocalQuaternion.ToRotationMatrix();
    EulerAngles angles{ 0.0f, 0.0f, 0.0f };
    std::tie(angles, std::ignore) = m_mxLocalRotation.ToEulerAnglesXYZ();
    m_vecLocalEulerAngle = Vector3(angles.m_x, angles.m_y, angles.m_z);
    m_mxLocalTransform = Matrix4::FromSRT(m_vecLocalScale, m_qtLocalQuaternion, m_vecLocalPosition);
    return _UpdateLocalTransform(m_mxLocalTransform);
}

error Spatial::SetLocalEulerAngle(const Vector3& angle)
{
    m_vecLocalEulerAngle = angle;
    m_mxLocalRotation = Matrix3::FromEulerAnglesXYZ({ m_vecLocalEulerAngle.X(), m_vecLocalEulerAngle.Y(), m_vecLocalEulerAngle.Z() });
    m_qtLocalQuaternion = Quaternion::FromRotationMatrix(m_mxLocalRotation);
    m_mxLocalTransform = Matrix4::FromSRT(m_vecLocalScale, m_qtLocalQuaternion, m_vecLocalPosition);
    return _UpdateLocalTransform(m_mxLocalTransform);
}

error Spatial::SetLocalScale(const MathLib::Vector3& scale)
{
    m_vecLocalScale = scale;
    m_mxLocalTransform = Matrix4::FromSRT(m_vecLocalScale, m_qtLocalQuaternion, m_vecLocalPosition);
    return _UpdateLocalTransform(m_mxLocalTransform);
}

error Spatial::SetLocalUniformScale(float scale)
{
    m_vecLocalScale = Vector3(scale, scale, scale);
    m_mxLocalTransform = Matrix4::FromSRT(m_vecLocalScale, m_qtLocalQuaternion, m_vecLocalPosition);
    return _UpdateLocalTransform(m_mxLocalTransform);
}

error Spatial::SetLocalTransform(const MathLib::Matrix4& mx)
{
    std::tie(m_vecLocalScale, m_qtLocalQuaternion, m_vecLocalPosition) = mx.UnMatrixSRT();
    m_mxLocalRotation = m_qtLocalQuaternion.ToRotationMatrix();
    EulerAngles angles{ 0.0f, 0.0f, 0.0f };
    std::tie(angles, std::ignore) = m_mxLocalRotation.ToEulerAnglesXYZ();
    m_vecLocalEulerAngle = Vector3(angles.m_x, angles.m_y, angles.m_z);
    return _UpdateLocalTransform(mx);
}

void Spatial::ChangeWorldPosition(const MathLib::Vector3& vecWorldPos, const std::shared_ptr<Node>& new_parent_node)
{
    Vector3 vecLocalPos = vecWorldPos;
    NodePtr targetParentNode = std::dynamic_pointer_cast<Node, Spatial>(GetParent());
    if (new_parent_node) targetParentNode = new_parent_node;  // if New Parent Node is 0, we no change parent node
    if (targetParentNode) // 有parent node, 取得local pos
    {
        Matrix4 mxNewParentWorldInv = targetParentNode->GetWorldTransform().Inverse();
        vecLocalPos = mxNewParentWorldInv.TransformCoord(vecWorldPos);
    }
    Matrix4 mxNewLocalTransform = m_mxLocalTransform;
    mxNewLocalTransform.SetColumn(3, Vector4(vecLocalPos.X(), vecLocalPos.Y(), vecLocalPos.Z(), 1.0f));
    // change parent node or not?
    if (GetParent() != targetParentNode)
    {
        NodePtr node = std::dynamic_pointer_cast<Node, Spatial>(GetParent());
        if (node) node->DetachChild(ThisSpatial());
        if (targetParentNode) targetParentNode->AttachChild(ThisSpatial(), mxNewLocalTransform);
    }
    else
    {
        SetLocalTransform(mxNewLocalTransform);
    }
}

SceneTraveler::TravelResult Spatial::VisitBy(SceneTraveler* traveler)
{
    if (!traveler) return SceneTraveler::TravelResult::InterruptError;
    return traveler->TravelTo(ThisSpatial());
}

void Spatial::SetCullingMode(CullingMode mode)
{
    bool has_changed = false;
    if (m_cullingMode != mode)
    {
        m_cullingMode = mode;
    }
    if ((TestNotifyFlag(Notify_CullMode)) && (has_changed))
    {
        Frameworks::EventPublisher::Post(std::make_shared<SpatialCullModeChanged>(ThisSpatial()));
    }
}

void Spatial::NotifySpatialRenderStateChanged()
{
    _PropagateSpatialRenderState();
}

error Spatial::_PropagateSpatialRenderState()
{
    if (!m_parent.expired()) return m_parent.lock()->_PropagateSpatialRenderState();
    return ErrorCode::ok;
}

error Spatial::_UpdateBoundData()
{
    if (m_modelBound.IsEmpty()) m_modelBound = Engine::BoundingVolume(Box3::UNIT_BOX);
    m_worldBound = Engine::BoundingVolume::CreateFromTransform(m_modelBound, m_mxWorldTransform);

    if (TestNotifyFlag(Notify_Bounding))
    {
        Frameworks::EventPublisher::Post(std::make_shared<SpatialBoundChanged>(ThisSpatial()));
    }

    error er = ErrorCode::ok;
    if (!m_parent.expired()) er = m_parent.lock()->_UpdateBoundData();

    return er;
}

error Spatial::_UpdateLocalTransform(const MathLib::Matrix4& mxLocal)
{
    m_mxLocalTransform = mxLocal;

    Matrix4 mxParent = Matrix4::IDENTITY;
    if (!m_parent.expired())
    {
        mxParent = m_parent.lock()->GetWorldTransform();
    }
    error er = _UpdateWorldData(mxParent);
    if (er) return er;

    if (TestNotifyFlag(Notify_Location))
    {
        Frameworks::EventPublisher::Post(std::make_shared<SpatialLocationChanged>(ThisSpatial()));
    }
    // propagate up
    er = _UpdateBoundData();

    return er;
}

error Spatial::_UpdateSpatialRenderState()
{
    if (!IsRenderable()) return ErrorCode::ok;  // only renderable entity need
    if (!(TestSpatialFlag(Spatial_Unlit))) m_spatialRenderState.QueryLightingState(m_vecWorldPosition);

    if (TestNotifyFlag(Notify_RenderState))
    {
        Frameworks::EventPublisher::Post(std::make_shared<SpatialRenderStateChanged>(ThisSpatial()));
    }
    return ErrorCode::ok;
}

error Spatial::_UpdateWorldData(const MathLib::Matrix4& mxParentWorld)
{
    m_mxWorldTransform = mxParentWorld * m_mxLocalTransform;
    if (m_modelBound.IsEmpty()) m_modelBound = Engine::BoundingVolume(Box3::UNIT_BOX);
    m_worldBound = Engine::BoundingVolume::CreateFromTransform(m_modelBound, m_mxWorldTransform);
    m_vecWorldPosition = m_mxWorldTransform.UnMatrixTranslate();
    _UpdateSpatialRenderState();
    return ErrorCode::ok;
}
