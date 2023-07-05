/*********************************************************************
 * \file   TerrainPrimitive.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   June 2023
 *********************************************************************/
#ifndef TERRAIN_PRIMITIVE_H
#define TERRAIN_PRIMITIVE_H

#include "Renderer/MeshPrimitive.h"

namespace Enigma::Terrain
{
    class TerrainPrimitive : public Renderer::MeshPrimitive
    {
        DECLARE_EN_RTTI;
    public:
        TerrainPrimitive(const std::string& name);
        virtual ~TerrainPrimitive() override;
        TerrainPrimitive(const TerrainPrimitive&);
        TerrainPrimitive& operator=(const TerrainPrimitive&);
        TerrainPrimitive(TerrainPrimitive&&) noexcept;
        TerrainPrimitive& operator=(TerrainPrimitive&&) noexcept;

        virtual Engine::GenericDto SerializeDto() override;
    };
}

#endif // TERRAIN_PRIMITIVE_H
