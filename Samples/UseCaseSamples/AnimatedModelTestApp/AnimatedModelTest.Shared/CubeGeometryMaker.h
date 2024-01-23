/*********************************************************************
 * \file   CubeGeometryMaker.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   December 2022
 *********************************************************************/
#ifndef _CUBE_GEOMETRY_MAKER_H
#define _CUBE_GEOMETRY_MAKER_H

#include "Geometries/GeometryId.h"
#include "GameEngine/GenericDto.h"

class CubeGeometryMaker
{
public:
    static void makeCube(const Enigma::Geometries::GeometryId& id);
};

#endif // _CUBE_GEOMETRY_MAKER_H
