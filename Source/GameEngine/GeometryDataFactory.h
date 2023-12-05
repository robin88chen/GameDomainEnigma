/*********************************************************************
 * \file   GeometryDataFactory.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   December 2023
 *********************************************************************/
#ifndef GEOMETRY_DATA_FACTORY_H
#define GEOMETRY_DATA_FACTORY_H

#include "GeometryId.h"
#include "GenericDto.h"
#include "Frameworks/Rtti.h"

namespace Enigma::Engine
{
    class GeometryData;

    class GeometryDataFactory
    {
    public:
        GeometryDataFactory();
        ~GeometryDataFactory();

        void registerHandlers();
        void unregisterHandlers();

        std::shared_ptr<GeometryData> create(const GeometryId& id, const Frameworks::Rtti& rtti);
        std::shared_ptr<GeometryData> constitute(const GeometryId& id, const GenericDto& dto);
    };
}

#endif // GEOMETRY_DATA_FACTORY_H
