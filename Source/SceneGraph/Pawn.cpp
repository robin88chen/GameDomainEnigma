#include "Pawn.h"
#include "Culler.h"
#include "SceneGraphErrors.h"
#include "GameEngine/BoundingVolume.h"
#include "SceneGraphDtos.h"
#include <cassert>

using namespace Enigma::SceneGraph;

DEFINE_RTTI(SceneGraph, Pawn, Spatial);

Pawn::Pawn(const std::string& name) : Spatial(name)
{
    m_factoryDesc = Engine::FactoryDesc(Pawn::TYPE_RTTI.GetName());
    m_primitive = nullptr;
    m_spatialFlags |=  (Spatial_ShadowCaster | Spatial_ShadowReceiver);
    RemoveSpatialFlag(Spatial_Unlit);
}

Pawn::Pawn(const PawnDto& dto) : Spatial(dynamic_cast<const SpatialDto&>(dto))
{
}

Pawn::~Pawn()
{
    m_primitive = nullptr;
}

error Pawn::OnCullingVisible(Culler* culler, bool noCull)
{
    assert(culler);
    culler->Insert(ThisSpatial());
    return ErrorCode::ok;
}

bool Pawn::IsRenderable()
{
    if (m_primitive) return true;
    return false;
}

error Pawn::InsertToRenderer(const Engine::IRendererPtr& render)
{
    assert(render);
    if (!m_primitive) return ErrorCode::nullPrimitive;

    error er = m_primitive->InsertToRendererWithTransformUpdating(render, m_mxWorldTransform, m_spatialRenderState.ToLightingState());
    return er;
}

void Pawn::SetPrimitive(const Engine::PrimitivePtr& prim)
{
    m_primitive = prim;

    // update local bound, world bound
    if ((m_primitive) && (!m_primitive->GetBoundingVolume().IsEmpty()))
    {
        m_modelBound = Engine::BoundingVolume{ m_primitive->GetBoundingVolume() };
    }

    _UpdateBoundData();
}

void Pawn::CalculateModelBound(bool axis_align)
{
    if (m_primitive)
    {
        m_primitive->CalculateBoundingVolume(axis_align);
    }
    // update local bound, world bound
    if ((m_primitive) && (!m_primitive->GetBoundingVolume().IsEmpty()))
    {
        m_modelBound = Engine::BoundingVolume{ m_primitive->GetBoundingVolume() };
    }

    _UpdateBoundData();
}

error Pawn::_UpdateLocalTransform(const MathLib::Matrix4& mxLocal)
{
    error er = Spatial::_UpdateLocalTransform(mxLocal);
    if (m_primitive) m_primitive->UpdateWorldTransform(m_mxWorldTransform);
    return er;
}

error Pawn::_UpdateWorldData(const MathLib::Matrix4& mxParentWorld)
{
    error er = Spatial::_UpdateWorldData(mxParentWorld);
    if (m_primitive) m_primitive->UpdateWorldTransform(m_mxWorldTransform);
    return er;
}

void Pawn::EnumAnimatorListDeep(std::list<std::shared_ptr<Engine::Animator>>& resultList)
{
    if (m_primitive) m_primitive->EnumAnimatorListDeep(resultList);
}
