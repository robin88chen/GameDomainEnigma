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
}

#endif // PRIMITIVE_QUERIES_H
