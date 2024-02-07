/*********************************************************************
 * \file   GeometryCreationHelper.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   February 2024
 *********************************************************************/
#ifndef GEOMETRY_CREATION_HELPER_H
#define GEOMETRY_CREATION_HELPER_H

#include "Geometries/GeometryId.h"
#include "Geometries/GeometryData.h"
#include <memory>

namespace EnigmaViewer
{
    class GeometryCreationHelper
    {
    public:
        static std::shared_ptr<Enigma::Geometries::GeometryData> createSquareXZQuad(const Enigma::Geometries::GeometryId& id);
    };
}

#endif // GEOMETRY_CREATION_HELPER_H
