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
        QueryPrimitiveNextSequenceNumber(const PrimitiveId& id) : m_id(id) {}

        const PrimitiveId& id() const { return m_id; }

    protected:
        PrimitiveId m_id;
    };
    class RequestPrimitiveCreation : public Frameworks::Query<std::shared_ptr<Primitive>>
    {
    public:
        enum class PersistenceLevel { None, Repository, Store };
    public:
        RequestPrimitiveCreation(const PrimitiveId& id, const Frameworks::Rtti& rtti, PersistenceLevel persistence_level) : m_id(id), m_rtti(rtti.getName()), m_persistenceLevel(persistence_level) {}
        const PrimitiveId& id() { return m_id; }
        const Frameworks::Rtti& rtti() { return Frameworks::Rtti::fromName(m_rtti); }
        PersistenceLevel persistenceLevel() const { return m_persistenceLevel; }

    private:
        PrimitiveId m_id;
        std::string m_rtti;
        PersistenceLevel m_persistenceLevel;
    };
    class RequestPrimitiveConstitution : public Frameworks::Query<std::shared_ptr<Primitive>>
    {
    public:
        enum class PersistenceLevel { None, Repository, Store };
    public:
        RequestPrimitiveConstitution(const PrimitiveId& id, const Engine::GenericDto& dto, PersistenceLevel persistence_level) : m_id(id), m_dto(dto), m_persistenceLevel(persistence_level) {}
        const PrimitiveId& id() { return m_id; }
        const Engine::GenericDto& dto() { return m_dto; }
        PersistenceLevel persistenceLevel() const { return m_persistenceLevel; }

    private:
        PrimitiveId m_id;
        Engine::GenericDto m_dto;
        PersistenceLevel m_persistenceLevel;
    };
}

#endif // PRIMITIVE_QUERIES_H
