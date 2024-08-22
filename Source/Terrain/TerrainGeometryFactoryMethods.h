/*****************************************************************
 * \file   TerrainGeometryFactoryMethods.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   August 2024
 ******************************************************************/
#ifndef TERRAIN_GEOMETRY_FACTORY_METHODS_H
#define TERRAIN_GEOMETRY_FACTORY_METHODS_H

#include "Geometries/GeometryId.h"
#include "GameEngine/GenericDto.h"
#include <memory>

namespace Enigma::Terrain
{
    class TerrainGeometry;

    std::shared_ptr<TerrainGeometry> createTerrainGeometry(const Geometries::GeometryId& id);
    std::shared_ptr<TerrainGeometry> constituteTerrainGeometry(const Geometries::GeometryId& id, const Engine::GenericDto& dto);
}

#endif // TERRAIN_GEOMETRY_FACTORY_METHODS_H
