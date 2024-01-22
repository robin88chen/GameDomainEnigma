/*********************************************************************
 * \file   PrimitiveHydratingPlan.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   January 2024
 *********************************************************************/
#ifndef PRIMITIVE_HYDRATING_PLAN_H
#define PRIMITIVE_HYDRATING_PLAN_H

#include "Primitives/PrimitiveId.h"
#include "Primitives/Primitive.h"
#include "GameEngine/GenericDto.h"

namespace Enigma::Renderables
{
    class PrimitiveHydratingPlan
    {
    public:
        PrimitiveHydratingPlan() : m_id() {};
        PrimitiveHydratingPlan(const Primitives::PrimitiveId& id, const std::shared_ptr<Primitives::Primitive>& prim, const Engine::GenericDto& dto) : m_id(id), m_primitive(prim), m_dto(dto) {}
        PrimitiveHydratingPlan(const PrimitiveHydratingPlan&) = default;
        PrimitiveHydratingPlan(PrimitiveHydratingPlan&&) = default;
        ~PrimitiveHydratingPlan() = default;
        PrimitiveHydratingPlan& operator=(const PrimitiveHydratingPlan&) = default;
        PrimitiveHydratingPlan& operator=(PrimitiveHydratingPlan&&) = default;

        const Primitives::PrimitiveId& id() const { return m_id; }
        const std::shared_ptr<Primitives::Primitive>& primitive() const { return m_primitive; }
        const Engine::GenericDto& dto() const { return m_dto; }

    protected:
        Primitives::PrimitiveId m_id;
        std::shared_ptr<Primitives::Primitive> m_primitive;
        Engine::GenericDto m_dto;
    };
}

#endif // PRIMITIVE_HYDRATING_PLAN_H
