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

Spatial::Spatial(const std::string& name) : m_factoryDesc(Spatial::TYPE_RTTI.getName())
{
    m_name = name;

    m_graphDepth = 0;

    //m_spatialRenderState = nullptr;

    m_cullingMode = CullingMode::Dynamic;

    m_spatialFlags = Spatial_Unlit; // 預設是不受光的...entity再打開

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

    m_notifyFlags = Notify_All;
}

Spatial::Spatial(const SpatialId& id) : m_factoryDesc(Spatial::TYPE_RTTI.getName()), m_id(id)
{
    m_name = id.name();

    m_graphDepth = 0;

    //m_spatialRenderState = nullptr;

    m_cullingMode = CullingMode::Dynamic;

    m_spatialFlags = Spatial_Unlit; // 預設是不受光的...entity再打開

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

    m_notifyFlags = Notify_All;

}

Spatial::Spatial(const GenericDto& o) : m_factoryDesc(o.getRtti())
{
    SpatialDto dto{ o };
    m_name = dto.name();
    m_graphDepth = dto.graphDepth();
    m_cullingMode = static_cast<CullingMode>(dto.cullingMode());
    m_spatialFlags = dto.spatialFlag();
    m_notifyFlags = dto.notifyFlag();
    m_mxLocalTransform = dto.localTransform();
    assert(m_mxLocalTransform != Matrix4::ZERO);
    m_mxWorldTransform = dto.worldTransform();
    assert(m_mxWorldTransform != Matrix4::ZERO);
    m_modelBound = BoundingVolume(BoundingVolumeDto::fromGenericDto(dto.modelBound()));
    m_worldBound = BoundingVolume(BoundingVolumeDto::fromGenericDto(dto.worldBound()));
    assert(!m_modelBound.IsEmpty());
    assert(!m_worldBound.IsEmpty());
    std::tie(m_vecLocalScale, m_qtLocalQuaternion, m_vecLocalPosition) = m_mxLocalTransform.UnMatrixSRT();
    m_mxLocalRotation = m_qtLocalQuaternion.ToRotationMatrix();
    EulerAngles angles{ 0.0f, 0.0f, 0.0f };
    std::tie(angles, std::ignore) = m_mxLocalRotation.ToEulerAnglesXYZ();
    m_vecLocalEulerAngle = Vector3(angles.m_x, angles.m_y, angles.m_z);
    m_vecWorldPosition = m_mxWorldTransform.UnMatrixTranslate();
}

Spatial::Spatial(const SpatialId& id, const GenericDto& o) : m_factoryDesc(o.getRtti()), m_id(id)
{
    SpatialDto dto{ o };
    m_name = dto.name();
    m_graphDepth = dto.graphDepth();
    m_cullingMode = static_cast<CullingMode>(dto.cullingMode());
    m_spatialFlags = dto.spatialFlag();
    m_notifyFlags = dto.notifyFlag();
    m_mxLocalTransform = dto.localTransform();
    assert(m_mxLocalTransform != Matrix4::ZERO);
    m_mxWorldTransform = dto.worldTransform();
    assert(m_mxWorldTransform != Matrix4::ZERO);
    m_modelBound = BoundingVolume(BoundingVolumeDto::fromGenericDto(dto.modelBound()));
    m_worldBound = BoundingVolume(BoundingVolumeDto::fromGenericDto(dto.worldBound()));
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

Enigma::Engine::GenericDto Spatial::serializeDto()
{
    return serializeSpatialDto().toGenericDto();
}

SpatialDto Spatial::serializeSpatialDto()
{
    SpatialDto dto;
    dto.factoryDesc() = m_factoryDesc;
    dto.name() = m_name;
    dto.id() = m_id;
    if (!m_parent.expired()) dto.parentName() = m_parent.lock()->getSpatialName();
    dto.graphDepth() = m_graphDepth;
    dto.cullingMode() = static_cast<unsigned int>(m_cullingMode);
    dto.spatialFlag() = static_cast<unsigned int>(m_spatialFlags.to_ulong());
    dto.notifyFlag() = static_cast<unsigned int>(m_notifyFlags.to_ulong());
    dto.localTransform() = m_mxLocalTransform;
    dto.worldTransform() = m_mxWorldTransform;
    dto.modelBound() = m_modelBound.serializeDto().toGenericDto();
    dto.worldBound() = m_worldBound.serializeDto().toGenericDto();
    return dto;
}

void Spatial::linkParent(const std::shared_ptr<Spatial>& parent)
{
    m_parent = parent;
    if ((!m_parent.expired()) && (m_parent.lock()))
        m_graphDepth = m_parent.lock()->getGraphDepth() + 1;
}

std::shared_ptr<Spatial> Spatial::getParent() const
{
    if (!m_parent.expired()) return m_parent.lock();
    return nullptr;
}

void Spatial::detachFromParent()
{
    if (m_parent.expired()) return;
    const NodePtr parent_node = std::dynamic_pointer_cast<Node, Spatial>(m_parent.lock());
    if (!parent_node) return;
    parent_node->detachChild(thisSpatial());
}

error Spatial::cullVisibleSet(Culler* culler, bool noCull)
{
    assert(culler);
    if (!canVisited()) return ErrorCode::dataNotReady;
    if (m_cullingMode == CullingMode::Always) return ErrorCode::ok;
    if (testSpatialFlag(Spatial_Hide)) return ErrorCode::ok;

    if (m_cullingMode == CullingMode::Never) noCull = true;

    error er = ErrorCode::ok;
    auto save_plane_activations = culler->GetPlaneActivations();
    if ((noCull) || (culler->IsVisible(m_worldBound)))
    {
        er = onCullingVisible(culler, noCull);
    }
    else if (culler->IsOutVisibility(m_worldBound))
    {
        onCullingCompleteNotVisible(culler);
    }

    culler->RestorePlaneBitFlags(save_plane_activations);

    return er;
}

Matrix4 Spatial::getParentWorldTransform() const
{
    if (!m_parent.expired()) return m_parent.lock()->getWorldTransform();
    return Matrix4::IDENTITY;
}

error Spatial::setLocalPosition(const Vector3& pos)
{
    m_vecLocalPosition = pos;
    m_mxLocalTransform.SetColumn(3, Vector4(pos.X(), pos.Y(), pos.Z(), 1.0f));
    return _updateLocalTransform(m_mxLocalTransform);
}

error Spatial::setLocalRotation(const Matrix3& rot)
{
    m_mxLocalRotation = rot;
    EulerAngles angles{ 0.0f, 0.0f, 0.0f };
    std::tie(angles, std::ignore) = m_mxLocalRotation.ToEulerAnglesXYZ();
    m_vecLocalEulerAngle = Vector3(angles.m_x, angles.m_y, angles.m_z);
    m_qtLocalQuaternion = Quaternion::FromRotationMatrix(m_mxLocalRotation);
    m_mxLocalTransform = Matrix4::FromSRT(m_vecLocalScale, m_qtLocalQuaternion, m_vecLocalPosition);
    return _updateLocalTransform(m_mxLocalTransform);
}

error Spatial::setLocalRotation(const Quaternion& qt)
{
    m_qtLocalQuaternion = qt;
    m_mxLocalRotation = m_qtLocalQuaternion.ToRotationMatrix();
    EulerAngles angles{ 0.0f, 0.0f, 0.0f };
    std::tie(angles, std::ignore) = m_mxLocalRotation.ToEulerAnglesXYZ();
    m_vecLocalEulerAngle = Vector3(angles.m_x, angles.m_y, angles.m_z);
    m_mxLocalTransform = Matrix4::FromSRT(m_vecLocalScale, m_qtLocalQuaternion, m_vecLocalPosition);
    return _updateLocalTransform(m_mxLocalTransform);
}

error Spatial::setLocalEulerAngle(const Vector3& angle)
{
    m_vecLocalEulerAngle = angle;
    m_mxLocalRotation = Matrix3::FromEulerAnglesXYZ({ m_vecLocalEulerAngle.X(), m_vecLocalEulerAngle.Y(), m_vecLocalEulerAngle.Z() });
    m_qtLocalQuaternion = Quaternion::FromRotationMatrix(m_mxLocalRotation);
    m_mxLocalTransform = Matrix4::FromSRT(m_vecLocalScale, m_qtLocalQuaternion, m_vecLocalPosition);
    return _updateLocalTransform(m_mxLocalTransform);
}

error Spatial::setLocalScale(const MathLib::Vector3& scale)
{
    m_vecLocalScale = scale;
    m_mxLocalTransform = Matrix4::FromSRT(m_vecLocalScale, m_qtLocalQuaternion, m_vecLocalPosition);
    return _updateLocalTransform(m_mxLocalTransform);
}

error Spatial::setLocalUniformScale(float scale)
{
    m_vecLocalScale = Vector3(scale, scale, scale);
    m_mxLocalTransform = Matrix4::FromSRT(m_vecLocalScale, m_qtLocalQuaternion, m_vecLocalPosition);
    return _updateLocalTransform(m_mxLocalTransform);
}

error Spatial::setLocalTransform(const MathLib::Matrix4& mx)
{
    std::tie(m_vecLocalScale, m_qtLocalQuaternion, m_vecLocalPosition) = mx.UnMatrixSRT();
    m_mxLocalRotation = m_qtLocalQuaternion.ToRotationMatrix();
    EulerAngles angles{ 0.0f, 0.0f, 0.0f };
    std::tie(angles, std::ignore) = m_mxLocalRotation.ToEulerAnglesXYZ();
    m_vecLocalEulerAngle = Vector3(angles.m_x, angles.m_y, angles.m_z);
    return _updateLocalTransform(mx);
}

void Spatial::changeWorldPosition(const MathLib::Vector3& vecWorldPos, const std::optional<std::shared_ptr<Node>>& new_parent_option)
{
    Vector3 vecLocalPos = vecWorldPos;
    NodePtr targetParentNode = std::dynamic_pointer_cast<Node, Spatial>(getParent());
    if (new_parent_option) targetParentNode = new_parent_option.value();  // if New Parent Node is null opt, we no change parent node
    if (targetParentNode) // 有parent node, 取得local pos
    {
        Matrix4 mxNewParentWorldInv = targetParentNode->getWorldTransform().Inverse();
        vecLocalPos = mxNewParentWorldInv.TransformCoord(vecWorldPos);
    }
    Matrix4 mxNewLocalTransform = m_mxLocalTransform;
    mxNewLocalTransform.SetColumn(3, Vector4(vecLocalPos.X(), vecLocalPos.Y(), vecLocalPos.Z(), 1.0f));
    // change parent node or not?
    if (getParent() != targetParentNode)
    {
        NodePtr node = std::dynamic_pointer_cast<Node, Spatial>(getParent());
        if (node) node->detachChild(thisSpatial());
        if (targetParentNode)
        {
            targetParentNode->attachChild(thisSpatial(), mxNewLocalTransform);
        }
        else
        {
            setLocalTransform(mxNewLocalTransform);
        }
    }
    else
    {
        setLocalTransform(mxNewLocalTransform);
    }
}

SceneTraveler::TravelResult Spatial::visitBy(SceneTraveler* traveler)
{
    if (!traveler) return SceneTraveler::TravelResult::InterruptError;
    return traveler->TravelTo(thisSpatial());
}

void Spatial::setCullingMode(CullingMode mode)
{
    bool has_changed = false;
    if (m_cullingMode != mode)
    {
        m_cullingMode = mode;
        has_changed = true;
    }
    if ((testNotifyFlag(Notify_CullMode)) && (has_changed))
    {
        Frameworks::EventPublisher::post(std::make_shared<SpatialCullModeChanged>(thisSpatial()));
    }
}

void Spatial::notifySpatialRenderStateChanged()
{
    _propagateSpatialRenderState();
}

error Spatial::_propagateSpatialRenderState()
{
    if (!m_parent.expired()) return m_parent.lock()->_propagateSpatialRenderState();
    return ErrorCode::ok;
}

error Spatial::_updateBoundData()
{
    if (m_modelBound.IsEmpty()) m_modelBound = Engine::BoundingVolume(Box3::UNIT_BOX);
    m_worldBound = Engine::BoundingVolume::CreateFromTransform(m_modelBound, m_mxWorldTransform);

    if (testNotifyFlag(Notify_Bounding))
    {
        Frameworks::EventPublisher::post(std::make_shared<SpatialBoundChanged>(thisSpatial()));
    }

    error er = ErrorCode::ok;
    if (!m_parent.expired()) er = m_parent.lock()->_updateBoundData();

    return er;
}

error Spatial::_updateLocalTransform(const MathLib::Matrix4& mxLocal)
{
    m_mxLocalTransform = mxLocal;

    Matrix4 mxParent = Matrix4::IDENTITY;
    if (!m_parent.expired())
    {
        mxParent = m_parent.lock()->getWorldTransform();
    }
    error er = _updateWorldData(mxParent);
    if (er) return er;

    if (testNotifyFlag(Notify_Location))
    {
        Frameworks::EventPublisher::post(std::make_shared<SpatialLocationChanged>(thisSpatial()));
    }
    // propagate up
    er = _updateBoundData();

    return er;
}

error Spatial::_updateSpatialRenderState()
{
    if (!isRenderable()) return ErrorCode::ok;  // only renderable entity need
    if (!(testSpatialFlag(Spatial_Unlit))) m_spatialRenderState.QueryLightingState(m_vecWorldPosition);

    if (testNotifyFlag(Notify_RenderState))
    {
        Frameworks::EventPublisher::post(std::make_shared<SpatialRenderStateChanged>(thisSpatial()));
    }
    return ErrorCode::ok;
}

error Spatial::_updateWorldData(const MathLib::Matrix4& mxParentWorld)
{
    m_mxWorldTransform = mxParentWorld * m_mxLocalTransform;
    if (m_modelBound.IsEmpty()) m_modelBound = Engine::BoundingVolume(Box3::UNIT_BOX);
    m_worldBound = Engine::BoundingVolume::CreateFromTransform(m_modelBound, m_mxWorldTransform);
    m_vecWorldPosition = m_mxWorldTransform.UnMatrixTranslate();
    _updateSpatialRenderState();
    return ErrorCode::ok;
}

void Spatial::addSpatialFlag(SpatialFlags flag)
{
    const bool visible_before = testSpatialFlag(SpatialBit::Spatial_Hide);
    m_spatialFlags |= flag;
    const bool visible_after = testSpatialFlag(SpatialBit::Spatial_Hide);
    if ((visible_before != visible_after) && (testNotifyFlag(Notify_Visibility)))
    {
        Frameworks::EventPublisher::post(std::make_shared<SpatialVisibilityChanged>(thisSpatial()));
    }
}

void Spatial::removeSpatialFlag(SpatialFlags flag)
{
    const bool visible_before = testSpatialFlag(SpatialBit::Spatial_Hide);
    m_spatialFlags &= (~flag);
    const bool visible_after = testSpatialFlag(SpatialBit::Spatial_Hide);
    if ((visible_before != visible_after) && (testNotifyFlag(Notify_Visibility)))
    {
        Frameworks::EventPublisher::post(std::make_shared<SpatialVisibilityChanged>(thisSpatial()));
    }
}
