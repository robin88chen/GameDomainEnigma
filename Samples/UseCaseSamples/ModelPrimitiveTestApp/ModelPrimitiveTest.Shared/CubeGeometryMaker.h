/*********************************************************************
 * \file   CubeGeometryMaker.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   December 2022
 *********************************************************************/
#ifndef _CUBE_GEOMETRY_MAKER_H
#define _CUBE_GEOMETRY_MAKER_H

#include "GameEngine/GeometryDataPolicy.h"

class CubeGeometryMaker
{
public:
    static void MakeSavedCube(const std::string& name);
    static Enigma::Engine::GeometryDataPolicy MakeGeometryPolicy(const std::string& name);
};

#endif // _CUBE_GEOMETRY_MAKER_H
