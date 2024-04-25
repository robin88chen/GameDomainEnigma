/*****************************************************************
 * \file   TerrainMaker.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   April 2024
 ******************************************************************/
#ifndef TERRAIN_MAKER_H
#define TERRAIN_MAKER_H

#include "Terrain/TerrainGeometry.h"
#include "Terrain/TerrainPrimitive.h"

class TerrainMaker
{
public:
    static Enigma::Engine::GenericDto makeTerrainGeometry(const Enigma::Geometries::GeometryId& id);
    static std::shared_ptr<Enigma::Terrain::TerrainPrimitive> makeTerrainPrimitive(const Enigma::Primitives::PrimitiveId& id, const Enigma::Geometries::GeometryId& geo_id);
};

#endif // TERRAIN_MAKER_H
