/*********************************************************************
 * \file   TerrainPrimitiveDto.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   July 2023
 *********************************************************************/
#ifndef TERRAIN_PRIMITIVE_DTO_H
#define TERRAIN_PRIMITIVE_DTO_H

#include "Renderer/RenderablePrimitiveDtos.h"

namespace Enigma::Terrain
{
    class TerrainPrimitivePolicy;

    class TerrainPrimitiveDto : public Renderer::MeshPrimitiveDto
    {
    public:
        TerrainPrimitiveDto();
        TerrainPrimitiveDto(const Renderer::MeshPrimitiveDto& dto);
        TerrainPrimitiveDto(const TerrainPrimitiveDto&) = default;
        TerrainPrimitiveDto(TerrainPrimitiveDto&&) = default;
        TerrainPrimitiveDto& operator=(const TerrainPrimitiveDto&) = default;
        TerrainPrimitiveDto& operator=(TerrainPrimitiveDto&&) = default;
        ~TerrainPrimitiveDto() = default;

        static TerrainPrimitiveDto FromGenericDto(const Engine::GenericDto& dto);
        Engine::GenericDto ToGenericDto() const;

        std::shared_ptr<Engine::GenericPolicy> ConvertToPolicy(const std::shared_ptr<Engine::IDtoDeserializer>&) const;
    };
}

#endif // TERRAIN_PRIMITIVE_DTO_H
