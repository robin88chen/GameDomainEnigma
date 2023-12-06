/*********************************************************************
 * \file   GeometryDataEvents.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   November 2022
 *********************************************************************/
#ifndef _GEOMETRY_DATA_EVENTS_H
#define _GEOMETRY_DATA_EVENTS_H

#include "Frameworks/Event.h"
#include "GeometryData.h"
#include <memory>
#include <system_error>

namespace Enigma::Engine
{
    class GeometryDataBuilt : public Frameworks::IEvent
    {
    public:
        GeometryDataBuilt(const GeometryId& id, const GeometryDataPtr geo) :
            m_id(id), m_geometry(geo) {};
        const GeometryId& id() { return m_id; }
        const GeometryDataPtr& geometryData() { return m_geometry; }
    private:
        GeometryId m_id;
        GeometryDataPtr m_geometry;
    };
    class BuildGeometryDataFailed : public Frameworks::IEvent
    {
    public:
        BuildGeometryDataFailed(const GeometryId& id, std::error_code er) :
            m_id(id), m_error(er) {};
        const GeometryId& id() { return m_id; }
        std::error_code error() const { return m_error; }
    private:
        GeometryId m_id;
        std::error_code m_error;
    };
    class FactoryGeometryCreated : public Frameworks::IEvent
    {
    public:
        FactoryGeometryCreated(const Engine::GenericDto& dto, const std::shared_ptr<GeometryData>& geometry)
            : m_dto(dto), m_geometry(geometry) {};

        const Engine::GenericDto& GetDto() const { return m_dto; }
        const std::shared_ptr<GeometryData>& GetGeometryData() { return m_geometry; }

    protected:
        Engine::GenericDto m_dto;
        std::shared_ptr<GeometryData> m_geometry;
    };
}

#endif // _GEOMETRY_DATA_EVENTS_H
