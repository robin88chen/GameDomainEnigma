#include "Spatial.h"
#include "SpatialAssembler.h"
#include "Node.h"
#include "SceneGraphErrors.h"
#include "Culler.h"
#include "SceneGraphEvents.h"
#include "SceneGraphDtos.h"
#include "MathLib/MathAlgorithm.h"
#include "Frameworks/EventPublisher.h"
#include "GameEngine/BoundingVolume.h"
#include "GameEngine/BoundingVolumeAssembler.h"
#include "SceneGraphQueries.h"
#include <cassert>
#include <tuple>

using namespace Enigma::SceneGraph;
using namespace Enigma::MathLib;
using namespace Enigma::Engine;

DEFINE_RTTI_OF_BASE(SceneGraph, Spatial);

Spatial::Spatial(const SpatialId& id) : m_factoryDesc(Spatial::TYPE_RTTI.getName()), m_id(id)
{
    m_graphDepth = 0;

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
/*Spatial::Spatial(const SpatialId& id, const GenericDto& o) : m_factoryDesc(o.getRtti()), m_id(id)
{
    SpatialDto dto{ o };
    m_graphDepth = dto.graphDepth();
    m_cullingMode = static_cast<CullingMode>(dto.cullingMode());
    m_spatialFlags = dto.spatialFlag();
    m_notifyFlags = dto.notifyFlag();
    m_mxLocalTransform = dto.localTransform();
    assert(m_mxLocalTransform != Matrix4::ZERO);
    m_mxWorldTransform = dto.worldTransform();
    assert(m_mxWorldTransform != Matrix4::ZERO);
    m_modelBound = BoundingVolume{ dto.modelBound() };
    m_worldBound = BoundingVolume{ dto.worldBound() };
    assert(!m_modelBound.isEmpty());
    assert(!m_worldBound.isEmpty());
    std::tie(m_vecLocalScale, m_qtLocalQuaternion, m_vecLocalPosition) = m_mxLocalTransform.UnMatrixSRT();
    m_mxLocalRotation = m_qtLocalQuaternion.ToRotationMatrix();
    EulerAngles angles{ 0.0f, 0.0f, 0.0f };
    std::tie(angles, std::ignore) = m_mxLocalRotation.ToEulerAnglesXYZ();
    m_vecLocalEulerAngle = Vector3(angles.m_x, angles.m_y, angles.m_z);
    m_vecWorldPosition = m_mxWorldTransform.UnMatrixTranslate();
}*/

Spatial::~Spatial()
{
}

/*Enigma::Engine::GenericDto Spatial::serializeDto()
{
    return serializeSpatialDto().toGenericDto();
}*/

std::shared_ptr<SpatialAssembler> Spatial::assembler() const
{
    return std::make_shared<SpatialAssembler>(m_id);
}

void Spatial::assemble(const std::shared_ptr<SpatialAssembler>& assembler)
{
    assert(assembler);
    assembler->factory(m_factoryDesc);
    assembler->localTransform(m_mxLocalTransform);
    assembler->worldTransform(m_mxWorldTransform);
    assembler->modelBound(m_modelBound);
    assembler->cullingMode(m_cullingMode);
    assembler->spatialFlags(m_spatialFlags);
    assembler->notifyFlags(m_notifyFlags);
    assembler->graphDepth(m_graphDepth);
    if (m_parent) assembler->parentId(m_parent.value());
}

std::shared_ptr<SpatialDisassembler> Spatial::disassembler() const
{
    return std::make_shared<SpatialDisassembler>();
}

void Spatial::disassemble(const std::shared_ptr<SpatialDisassembler>& disassembler)
{
    assert(disassembler);
    assert(m_id == disassembler->id());
    m_factoryDesc = disassembler->factory();
    m_graphDepth = disassembler->graphDepth();
    m_cullingMode = disassembler->cullingMode();
    m_spatialFlags = disassembler->spatialFlags();
    m_notifyFlags = disassembler->notifyFlags();
    m_mxLocalTransform = disassembler->localTransform();
    assert(m_mxLocalTransform != Matrix4::ZERO);
    m_mxWorldTransform = disassembler->worldTransform();
    assert(m_mxWorldTransform != Matrix4::ZERO);
    m_modelBound = disassembler->modelBound();
    m_worldBound = disassembler->worldBound();
    assert(!m_modelBound.isEmpty());
    assert(!m_worldBound.isEmpty());
    std::tie(m_vecLocalScale, m_qtLocalQuaternion, m_vecLocalPosition) = m_mxLocalTransform.UnMatrixSRT();
    m_mxLocalRotation = m_qtLocalQuaternion.ToRotationMatrix();
    EulerAngles angles{ 0.0f, 0.0f, 0.0f };
    std::tie(angles, std::ignore) = m_mxLocalRotation.ToEulerAnglesXYZ();
    m_vecLocalEulerAngle = Vector3(angles.m_x, angles.m_y, angles.m_z);
    m_vecWorldPosition = m_mxWorldTransform.UnMatrixTranslate();
}

std::shared_ptr<Spatial> Spatial::querySpatial(const SpatialId& id)
{
    assert(id.rtti().isDerived(Spatial::TYPE_RTTI));
    return std::make_shared<QuerySpatial>(id)->dispatch();
}

/*SpatialDto Spatial::serializeSpatialDto()
{
    SpatialDto dto;
    dto.factoryDesc(m_factoryDesc);
    //dto.name() = m_name;
    dto.id(m_id);
    if (m_parent) dto.parentName(m_parent.value().name());
    //dto.graphDepth() = m_graphDepth;
    dto.cullingMode(static_cast<unsigned int>(m_cullingMode));
    dto.spatialFlag(static_cast<unsigned int>(m_spatialFlags.to_ulong()));
    dto.notifyFlag(static_cast<unsigned int>(m_notifyFlags.to_ulong()));
    dto.localTransform(m_mxLocalTransform);
    dto.worldTransform(m_mxWorldTransform);
    std::shared_ptr<BoundingVolumeAssembler> model_assembler = std::make_shared<BoundingVolumeAssembler>();
    m_modelBound.assemble(model_assembler);
    std::shared_ptr<BoundingVolumeAssembler> world_assembler = std::make_shared<BoundingVolumeAssembler>();
    m_worldBound.assemble(world_assembler);
    dto.modelBound(model_assembler->assemble());
    dto.worldBound(world_assembler->assemble());
    return dto;
}*/

void Spatial::linkParent(const std::optional<SpatialId>& parent)
{
    m_parent = parent;
    if (auto p = getParent()) m_graphDepth = p->getGraphDepth() + 1;
}

std::shared_ptr<Spatial> Spatial::getParent() const
{
    if (!m_parent.has_value()) return nullptr;
    return std::make_shared<QuerySpatial>(m_parent.value())->dispatch();
}

void Spatial::detachFromParent()
{
    if (!m_parent.has_value()) return;
    const NodePtr parent_node = Node::queryNode(m_parent.value());
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
    if (auto parent = getParent()) return parent->getWorldTransform();
    return Matrix4::IDENTITY;
}

error Spatial::setLocalPosition(const Vector3& pos)
{
    m_vecLocalPosition = pos;
    m_mxLocalTransform.SetColumn(3, Vector4(pos.x(), pos.y(), pos.z(), 1.0f));
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
    m_mxLocalRotation = Matrix3::FromEulerAnglesXYZ({ m_vecLocalEulerAngle.x(), m_vecLocalEulerAngle.y(), m_vecLocalEulerAngle.z() });
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
    mxNewLocalTransform.SetColumn(3, Vector4(vecLocalPos.x(), vecLocalPos.y(), vecLocalPos.z(), 1.0f));
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
    return traveler->travelTo(thisSpatial());
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
        Frameworks::EventPublisher::enqueue(std::make_shared<SpatialCullModeChanged>(m_id));
    }
}

void Spatial::notifySpatialRenderStateChanged()
{
    _propagateSpatialRenderState();
}

error Spatial::_propagateSpatialRenderState()
{
    if (auto parent = getParent()) return getParent()->_propagateSpatialRenderState();
    return ErrorCode::ok;
}

error Spatial::_updateBoundData()
{
    if (m_modelBound.isEmpty()) m_modelBound = Engine::BoundingVolume(Box3::UNIT_BOX);
    m_worldBound = Engine::BoundingVolume::CreateFromTransform(m_modelBound, m_mxWorldTransform);

    if (testNotifyFlag(Notify_Bounding))
    {
        Frameworks::EventPublisher::enqueue(std::make_shared<SpatialBoundChanged>(m_id));
    }

    error er = ErrorCode::ok;
    if (auto parent = getParent()) er = parent->_updateBoundData();

    return er;
}

error Spatial::_updateLocalTransform(const MathLib::Matrix4& mxLocal)
{
    m_mxLocalTransform = mxLocal;

    Matrix4 mxParent = Matrix4::IDENTITY;
    if (auto parent = getParent())
    {
        mxParent = parent->getWorldTransform();
    }
    error er = _updateWorldData(mxParent);
    if (er) return er;

    if (testNotifyFlag(Notify_Location))
    {
        Frameworks::EventPublisher::enqueue(std::make_shared<SpatialLocationChanged>(m_id));
    }
    // propagate up
    er = _updateBoundData();

    return er;
}

error Spatial::_updateSpatialRenderState()
{
    if (!isRenderable()) return ErrorCode::ok;  // only renderable entity need
    if (!(testSpatialFlag(Spatial_Unlit)))
    {
        m_spatialRenderState = std::make_shared<QueryLightingStateAt>(m_vecWorldPosition)->dispatch();
    }
    if (testNotifyFlag(Notify_RenderState))
    {
        Frameworks::EventPublisher::enqueue(std::make_shared<SpatialRenderStateChanged>(m_id));
    }
    return ErrorCode::ok;
}

error Spatial::_updateWorldData(const MathLib::Matrix4& mxParentWorld)
{
    m_mxWorldTransform = mxParentWorld * m_mxLocalTransform;
    if (m_modelBound.isEmpty()) m_modelBound = Engine::BoundingVolume(Box3::UNIT_BOX);
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
        Frameworks::EventPublisher::enqueue(std::make_shared<SpatialVisibilityChanged>(m_id));
    }
}

void Spatial::removeSpatialFlag(SpatialFlags flag)
{
    const bool visible_before = testSpatialFlag(SpatialBit::Spatial_Hide);
    m_spatialFlags &= (~flag);
    const bool visible_after = testSpatialFlag(SpatialBit::Spatial_Hide);
    if ((visible_before != visible_after) && (testNotifyFlag(Notify_Visibility)))
    {
        Frameworks::EventPublisher::enqueue(std::make_shared<SpatialVisibilityChanged>(m_id));
    }
}
