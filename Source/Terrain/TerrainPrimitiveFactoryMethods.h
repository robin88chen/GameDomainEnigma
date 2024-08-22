/*****************************************************************
 * \file   TerrainPrimitiveFactoryMethods.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   August 2024
 ******************************************************************/
#ifndef TERRAIN_PRIMITIVE_FACTORY_METHODS_H
#define TERRAIN_PRIMITIVE_FACTORY_METHODS_H

#include "Primitives/PrimitiveId.h"
#include "GameEngine/GenericDto.h"
#include <memory>

namespace Enigma::Terrain
{
    class TerrainPrimitive;

    std::shared_ptr<TerrainPrimitive> createTerrainPrimitive(const Primitives::PrimitiveId& id);
    std::shared_ptr<TerrainPrimitive> constituteTerrainPrimitive(const Primitives::PrimitiveId& id, const Engine::GenericDto& dto);
}

#endif // TERRAIN_PRIMITIVE_FACTORY_METHODS_H
