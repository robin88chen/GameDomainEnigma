/*********************************************************************
 * \file   TerrainPrimitiveDto.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   July 2023
 *********************************************************************/
#ifndef TERRAIN_PRIMITIVE_DTO_H
#define TERRAIN_PRIMITIVE_DTO_H

#include "Renderables/RenderablePrimitiveDtos.h"

namespace Enigma::Terrain
{
    class TerrainPrimitivePolicy;

    class TerrainPrimitiveDto : public Renderables::MeshPrimitiveDto
    {
    public:
        TerrainPrimitiveDto();
        TerrainPrimitiveDto(const Engine::GenericDto& dto);
        TerrainPrimitiveDto(const Renderables::MeshPrimitiveDto& dto);
        TerrainPrimitiveDto(const TerrainPrimitiveDto&) = default;
        TerrainPrimitiveDto(TerrainPrimitiveDto&&) = default;
        TerrainPrimitiveDto& operator=(const TerrainPrimitiveDto&) = default;
        TerrainPrimitiveDto& operator=(TerrainPrimitiveDto&&) = default;
        ~TerrainPrimitiveDto() = default;

        Engine::GenericDto toGenericDto() const;
    };
}

#endif // TERRAIN_PRIMITIVE_DTO_H
