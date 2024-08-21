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
#include "Frameworks/Rtti.h"
#include <memory>

namespace Enigma::Geometries
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
    class HasGeometryData : public Frameworks::Query<bool>
    {
    public:
        HasGeometryData(const GeometryId& id) : m_id(id) {}

        const GeometryId& id() const { return m_id; }

    protected:
        GeometryId m_id;
    };
    class RequestGeometryCreation : public Frameworks::Query<std::shared_ptr<GeometryData>>
    {
    public:
        RequestGeometryCreation(const GeometryId& id, const Frameworks::Rtti& rtti) : m_id(id), m_rtti(rtti.getName()) {}
        const GeometryId& id() { return m_id; }
        const Frameworks::Rtti& rtti() { return Frameworks::Rtti::fromName(m_rtti); }

    private:
        GeometryId m_id;
        std::string m_rtti;
    };
    class RequestGeometryConstitution : public Frameworks::Query<std::shared_ptr<GeometryData>>
    {
    public:
        RequestGeometryConstitution(const GeometryId& id, const Engine::GenericDto& dto) : m_id(id), m_dto(dto) {}
        const GeometryId& id() { return m_id; }
        const Engine::GenericDto& dto() { return m_dto; }

    private:
        GeometryId m_id;
        Engine::GenericDto m_dto;
    };
}

#endif // GEOMETRY_DATA_QUERIES_H
