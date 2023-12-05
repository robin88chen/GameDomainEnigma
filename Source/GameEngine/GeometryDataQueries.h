/*********************************************************************
 * \file   GeometryDataQueries.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   December 2023
 *********************************************************************/
#ifndef GEOMETRY_DATA_QUERIES_H
#define GEOMETRY_DATA_QUERIES_H

#include "Frameworks/Query.h"
#include "GeometryId.h"
#include <memory>

namespace Enigma::Engine
{
    class GeometryData;

    class QueryGeometryData : public Frameworks::Query<std::shared_ptr<GeometryData>>
    {
    public:
        QueryGeometryData(const GeometryId& id) : m_id(id) {}

        const GeometryId& id() const { return m_id; }

    protected:
        GeometryId m_id;
    };
}

#endif // GEOMETRY_DATA_QUERIES_H
