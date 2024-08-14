/*********************************************************************
 * \file   PrimitiveQueries.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   December 2023
 *********************************************************************/
#ifndef PRIMITIVE_QUERIES_H
#define PRIMITIVE_QUERIES_H

#include "Frameworks/Query.h"
#include "PrimitiveId.h"
#include "GameEngine/GenericDto.h"

namespace Enigma::Primitives
{
    class Primitive;

    class QueryPrimitive : public Frameworks::Query<std::shared_ptr<Primitive>>
    {
    public:
        QueryPrimitive(const PrimitiveId& id) : m_id(id) {}

        const PrimitiveId& id() const { return m_id; }

    protected:
        PrimitiveId m_id;
    };
    class QueryPrimitiveNextSequenceNumber : public Frameworks::Query<std::uint64_t>
    {
    public:
        QueryPrimitiveNextSequenceNumber(const PrimitiveId& id) : Query(0), m_id(id) {}

        const PrimitiveId& id() const { return m_id; }

    protected:
        PrimitiveId m_id;
    };
    class RequestPrimitiveCreation : public Frameworks::Query<std::shared_ptr<Primitive>>
    {
    public:
        RequestPrimitiveCreation(const PrimitiveId& id, const Frameworks::Rtti& rtti) : m_id(id), m_rtti(rtti.getName()) {}
        const PrimitiveId& id() { return m_id; }
        const Frameworks::Rtti& rtti() { return Frameworks::Rtti::fromName(m_rtti); }

    private:
        PrimitiveId m_id;
        std::string m_rtti;
    };
    class RequestPrimitiveConstitution : public Frameworks::Query<std::shared_ptr<Primitive>>
    {
    public:
        RequestPrimitiveConstitution(const PrimitiveId& id, const Engine::GenericDto& dto) : m_id(id), m_dto(dto) {}
        const PrimitiveId& id() { return m_id; }
        const Engine::GenericDto& dto() { return m_dto; }

    private:
        PrimitiveId m_id;
        Engine::GenericDto m_dto;
    };
    class QueryPrimitiveDto : public Frameworks::Query<std::optional<Engine::GenericDto>>
    {
    public:
        QueryPrimitiveDto(const PrimitiveId& id) : m_id(id) {}
        const PrimitiveId& id() { return m_id; }
    protected:
        PrimitiveId m_id;
    };
}

#endif // PRIMITIVE_QUERIES_H
