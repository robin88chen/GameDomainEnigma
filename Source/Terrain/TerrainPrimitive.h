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
        TerrainPrimitive(const Primitives::PrimitiveId& id);
        virtual ~TerrainPrimitive() override;
        TerrainPrimitive(const TerrainPrimitive&) = delete;
        TerrainPrimitive& operator=(const TerrainPrimitive&) = delete;
        TerrainPrimitive(TerrainPrimitive&&) = delete;
        TerrainPrimitive& operator=(TerrainPrimitive&&) = delete;

        virtual Engine::GenericDto serializeDto() const override;
    };
}

#endif // TERRAIN_PRIMITIVE_H
