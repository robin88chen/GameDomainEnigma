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
    class GeometryDataCreated : public Frameworks::IEvent
    {
    public:
        GeometryDataCreated(const std::string& name, const GeometryDataPtr geo) :
            m_name(name), m_geometry(geo) {};
        const std::string& GetName() { return m_name; }
        const GeometryDataPtr& GetGeometryData() { return m_geometry; }
    private:
        std::string m_name;
        GeometryDataPtr m_geometry;
    };
}

#endif // _GEOMETRY_DATA_EVENTS_H
