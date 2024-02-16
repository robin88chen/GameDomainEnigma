/*********************************************************************
 * \file   CubeGeometryMaker.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   December 2022
 *********************************************************************/
#ifndef _CUBE_GEOMETRY_MAKER_H
#define _CUBE_GEOMETRY_MAKER_H

#include "Geometries/GeometryData.h"
#include "Geometries/GeometryId.h"

class CubeGeometryMaker
{
public:
    static std::shared_ptr<Enigma::Geometries::GeometryData> makeCube(const Enigma::Geometries::GeometryId& id);
};

#endif // _CUBE_GEOMETRY_MAKER_H
