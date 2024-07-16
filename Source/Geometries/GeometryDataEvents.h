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

namespace Enigma::Geometries
{
    class GeometryCreated : public Frameworks::IEvent
    {
    public:
        GeometryCreated(const GeometryId& id, const GeometryDataPtr& geo) :
            m_id(id), m_geometry(geo) {};
        const GeometryId& id() { return m_id; }
        const GeometryDataPtr& geometryData() { return m_geometry; }
    private:
        GeometryId m_id;
        GeometryDataPtr m_geometry;
    };
    class CreateGeometryFailed : public Frameworks::IEvent
    {
    public:
        CreateGeometryFailed(const GeometryId& id, std::error_code er) :
            m_id(id), m_error(er) {};
        const GeometryId& id() { return m_id; }
        std::error_code error() const { return m_error; }
    private:
        GeometryId m_id;
        std::error_code m_error;
    };
    class GeometryConstituted : public Frameworks::IEvent
    {
    public:
        GeometryConstituted(const GeometryId& id, const GeometryDataPtr& geo, bool is_persisted) :
            m_id(id), m_geometry(geo), m_is_persisted(is_persisted) {};
        const GeometryId& id() { return m_id; }
        const GeometryDataPtr& geometryData() { return m_geometry; }
        bool isPersisted() const { return m_is_persisted; }
    private:
        GeometryId m_id;
        GeometryDataPtr m_geometry;
        bool m_is_persisted;
    };
    class ConstituteGeometryFailed : public Frameworks::IEvent
    {
    public:
        ConstituteGeometryFailed(const GeometryId& id, std::error_code er) :
            m_id(id), m_error(er) {};
        const GeometryId& id() { return m_id; }
        std::error_code error() const { return m_error; }
    private:
        GeometryId m_id;
        std::error_code m_error;
    };
    class GeometryRemoved : public Frameworks::IEvent
    {
    public:
        GeometryRemoved(const GeometryId& id) : m_id(id) {};
        const GeometryId& id() { return m_id; }
    private:
        GeometryId m_id;
    };
    class RemoveGeometryFailed : public Frameworks::IEvent
    {
    public:
        RemoveGeometryFailed(const GeometryId& id, std::error_code er) :
            m_id(id), m_error(er) {};
        const GeometryId& id() { return m_id; }
        std::error_code error() const { return m_error; }
    private:
        GeometryId m_id;
        std::error_code m_error;
    };
    class GeometryPut : public Frameworks::IEvent
    {
    public:
        GeometryPut(const GeometryId& id) : m_id(id) {};
        const GeometryId& id() { return m_id; }
    private:
        GeometryId m_id;
    };
    class PutGeometryFailed : public Frameworks::IEvent
    {
    public:
        PutGeometryFailed(const GeometryId& id, std::error_code er) :
            m_id(id), m_error(er) {};
        const GeometryId& id() { return m_id; }
        std::error_code error() const { return m_error; }
    private:
        GeometryId m_id;
        std::error_code m_error;
    };
    class GeometryReleased : public Frameworks::IEvent
    {
    public:
        GeometryReleased(const GeometryId& id) : m_id(id) {};
        const GeometryId& id() { return m_id; }
    private:
        GeometryId m_id;
    };
    class ReleaseGeometryFailed : public Frameworks::IEvent
    {
    public:
        ReleaseGeometryFailed(const GeometryId& id, std::error_code er) :
            m_id(id), m_error(er) {}
        const GeometryId& id() { return m_id; }
        std::error_code error() const { return m_error; }
    private:
        GeometryId m_id;
        std::error_code m_error;
    };
}

#endif // _GEOMETRY_DATA_EVENTS_H
