/*********************************************************************
 * \file   PrimitiveBuildingPlan.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   January 2024
 *********************************************************************/
#ifndef PRIMITIVE_BUILDING_PLAN_H
#define PRIMITIVE_BUILDING_PLAN_H

#include "Primitives/PrimitiveId.h"
#include "Primitives/Primitive.h"
#include "GameEngine/GenericDto.h"

namespace Enigma::Renderables
{
    class PrimitiveBuildingPlan
    {
    public:
        PrimitiveBuildingPlan() : m_id() {};
        PrimitiveBuildingPlan(const Primitives::PrimitiveId& id, const std::shared_ptr<Primitives::Primitive>& prim, const Engine::GenericDto& dto) : m_id(id), m_primitive(prim), m_dto(dto) {}
        PrimitiveBuildingPlan(const PrimitiveBuildingPlan&) = default;
        PrimitiveBuildingPlan(PrimitiveBuildingPlan&&) = default;
        ~PrimitiveBuildingPlan() = default;
        PrimitiveBuildingPlan& operator=(const PrimitiveBuildingPlan&) = default;
        PrimitiveBuildingPlan& operator=(PrimitiveBuildingPlan&&) = default;

        const Primitives::PrimitiveId& id() const { return m_id; }
        const std::shared_ptr<Primitives::Primitive>& primitive() const { return m_primitive; }
        const Engine::GenericDto& dto() const { return m_dto; }

    protected:
        Primitives::PrimitiveId m_id;
        std::shared_ptr<Primitives::Primitive> m_primitive;
        Engine::GenericDto m_dto;
    };
}

#endif // PRIMITIVE_BUILDING_PLAN_H
