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
    class RequestGeometryCreation : public Frameworks::Query<std::shared_ptr<GeometryData>>
    {
    public:
        enum class PersistenceLevel { None, Repository, Store };
    public:
        RequestGeometryCreation(const GeometryId& id, const Frameworks::Rtti& rtti, PersistenceLevel persistence_level) : m_id(id), m_rtti(rtti.getName()), m_persistence_level(persistence_level) {}
        const GeometryId& id() { return m_id; }
        const Frameworks::Rtti& rtti() { return Frameworks::Rtti::fromName(m_rtti); }
        PersistenceLevel persistenceLevel() const { return m_persistence_level; }

    private:
        GeometryId m_id;
        std::string m_rtti;
        PersistenceLevel m_persistence_level;
    };
    class RequestGeometryConstitution : public Frameworks::Query<std::shared_ptr<GeometryData>>
    {
    public:
        enum class PersistenceLevel { None, Repository, Store };
    public:
        RequestGeometryConstitution(const GeometryId& id, const Engine::GenericDto& dto, PersistenceLevel persistence_level) : m_id(id), m_dto(dto), m_persistence_level(persistence_level) {}
        const GeometryId& id() { return m_id; }
        const Engine::GenericDto& dto() { return m_dto; }
        PersistenceLevel persistenceLevel() const { return m_persistence_level; }

    private:
        GeometryId m_id;
        Engine::GenericDto m_dto;
        PersistenceLevel m_persistence_level;
    };
}

#endif // GEOMETRY_DATA_QUERIES_H
