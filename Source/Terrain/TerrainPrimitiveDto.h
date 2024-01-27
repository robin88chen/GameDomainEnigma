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
        TerrainPrimitiveDto(const Renderables::MeshPrimitiveDto& dto);
        TerrainPrimitiveDto(const TerrainPrimitiveDto&) = default;
        TerrainPrimitiveDto(TerrainPrimitiveDto&&) = default;
        TerrainPrimitiveDto& operator=(const TerrainPrimitiveDto&) = default;
        TerrainPrimitiveDto& operator=(TerrainPrimitiveDto&&) = default;
        ~TerrainPrimitiveDto() = default;

        static TerrainPrimitiveDto fromGenericDto(const Engine::GenericDto& dto);
        Engine::GenericDto toGenericDto() const;

        static std::shared_ptr<Engine::GenericPolicy> TerrainMeshConvertToPolicy(const Engine::GenericDto&, const std::shared_ptr<Engine::IDtoDeserializer>&);
    };
}

#endif // TERRAIN_PRIMITIVE_DTO_H
