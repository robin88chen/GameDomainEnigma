#include "Pawn.h"
#include "Culler.h"
#include "SceneGraphErrors.h"
#include "GameEngine/BoundingVolume.h"
#include "PawnAssembler.h"
#include "Frameworks/CommandBus.h"
#include "Primitives/Primitive.h"
#include "SceneGraphQueries.h"
#include <cassert>

using namespace Enigma::SceneGraph;

DEFINE_RTTI(SceneGraph, Pawn, Spatial);

Pawn::Pawn(const SpatialId& id) : Spatial(id)
{
    m_factoryDesc = Engine::FactoryDesc(Pawn::TYPE_RTTI.getName());
    m_primitive = nullptr;
    removeSpatialFlag(Spatial_Unlit);
}

/*Pawn::Pawn(const SpatialId& id, const Engine::GenericDto& dto) : Spatial(id, dto)
{
    PawnDto pawn_dto{ dto };
    if (pawn_dto.primitiveId()) m_primitive = Primitives::Primitive::queryPrimitive(pawn_dto.primitiveId().value());
}*/

Pawn::~Pawn()
{
    m_primitive = nullptr;
}

std::shared_ptr<Pawn> Pawn::create(const SpatialId& id)
{
    return std::make_shared<Pawn>(id);
}

std::shared_ptr<SpatialAssembler> Pawn::assembler() const
{
    return std::make_shared<PawnAssembler>(m_id);
}

void Pawn::assemble(const std::shared_ptr<SpatialAssembler>& assembler)
{
    assert(assembler);
    Spatial::assemble(assembler);
    if (auto pawn_assembler = std::dynamic_pointer_cast<PawnAssembler>(assembler))
    {
        if (m_primitive)
        {
            pawn_assembler->primitiveId(m_primitive->id());
        }
    }
}

std::shared_ptr<SpatialDisassembler> Pawn::disassembler() const
{
    return std::make_shared<PawnDisassembler>();
}

void Pawn::disassemble(const std::shared_ptr<SpatialDisassembler>& disassembler)
{
    assert(disassembler);
    Spatial::disassemble(disassembler);
    if (auto pawn_disassembler = std::dynamic_pointer_cast<PawnDisassembler>(disassembler))
    {
        if (pawn_disassembler->primitiveId())
        {
            m_primitive = Primitives::Primitive::queryPrimitive(pawn_disassembler->primitiveId().value());
        }
    }
}

/*std::shared_ptr<Pawn> Pawn::create(const SpatialId& id)
{
    return std::make_shared<Pawn>(id);
}

std::shared_ptr<Pawn> Pawn::constitute(const SpatialId& id, const Engine::GenericDto& dto)
{
    return std::make_shared<Pawn>(id, dto);
}*/

std::shared_ptr<Pawn> Pawn::queryPawn(const SpatialId& id)
{
    assert(id.rtti().isDerived(Pawn::TYPE_RTTI));
    return std::dynamic_pointer_cast<Pawn>(std::make_shared<QuerySpatial>(id)->dispatch());
}

/*Enigma::Engine::GenericDto Pawn::serializeDto()
{
    return SerializePawnDto().toGenericDto();
}

PawnDto Pawn::SerializePawnDto()
{
    PawnDto dto(serializeSpatialDto());
    if (m_primitive)
    {
        dto.primitiveId(m_primitive->id());
    }
    return dto;
}*/

error Pawn::onCullingVisible(Culler* culler, bool noCull)
{
    assert(culler);
    culler->Insert(thisSpatial());
    return ErrorCode::ok;
}

bool Pawn::isRenderable()
{
    if (m_primitive) return true;
    return false;
}

error Pawn::insertToRenderer(const Engine::IRendererPtr& render)
{
    assert(render);
    if (!m_primitive) return ErrorCode::nullPrimitive;

    error er = m_primitive->insertToRendererWithTransformUpdating(render, m_mxWorldTransform, m_spatialRenderState.lightingState());
    return er;
}

void Pawn::SetPrimitive(const std::shared_ptr<Primitives::Primitive>& prim)
{
    m_primitive = prim;

    // update local bound, world bound
    if ((m_primitive) && (!m_primitive->getBoundingVolume().isEmpty()))
    {
        m_modelBound = Engine::BoundingVolume{ m_primitive->getBoundingVolume() };
    }

    _updateBoundData();
    _propagateSpatialRenderState();
}

void Pawn::CalculateModelBound(bool axis_align)
{
    if (m_primitive)
    {
        m_primitive->calculateBoundingVolume(axis_align);
    }
    // update local bound, world bound
    if ((m_primitive) && (!m_primitive->getBoundingVolume().isEmpty()))
    {
        m_modelBound = Engine::BoundingVolume{ m_primitive->getBoundingVolume() };
    }

    _updateBoundData();
}

error Pawn::_updateLocalTransform(const MathLib::Matrix4& mxLocal)
{
    error er = Spatial::_updateLocalTransform(mxLocal);
    if (m_primitive) m_primitive->updateWorldTransform(m_mxWorldTransform);
    return er;
}

error Pawn::_updateWorldData(const MathLib::Matrix4& mxParentWorld)
{
    error er = Spatial::_updateWorldData(mxParentWorld);
    if (m_primitive) m_primitive->updateWorldTransform(m_mxWorldTransform);
    return er;
}

void Pawn::enumAnimatorListDeep(std::list<std::shared_ptr<Animators::Animator>>& resultList)
{
    if (m_primitive) m_primitive->enumAnimatorListDeep(resultList);
}
