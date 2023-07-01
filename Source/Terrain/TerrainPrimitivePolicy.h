/*********************************************************************
 * \file   TerrainPrimitivePolicy.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   July 2023
 *********************************************************************/
#ifndef TERRAIN_PRIMITIVE_POLICY_H
#define TERRAIN_PRIMITIVE_POLICY_H

#include "Renderer/RenderablePrimitivePolicies.h"

namespace Enigma::Terrain
{
    class TerrainPrimitivePolicy: public Renderer::MeshPrimitivePolicy
    {
    public:
        TerrainPrimitivePolicy();
        TerrainPrimitivePolicy(const TerrainPrimitivePolicy&) = default;
        TerrainPrimitivePolicy(TerrainPrimitivePolicy&&) = default;
        TerrainPrimitivePolicy& operator=(const TerrainPrimitivePolicy&) = default;
        TerrainPrimitivePolicy& operator=(TerrainPrimitivePolicy&&) = default;
        ~TerrainPrimitivePolicy() = default;

        virtual std::shared_ptr<Renderer::MeshPrimitive> CreatePrimitive() const override;
    };
}
#endif // TERRAIN_PRIMITIVE_POLICY_H
