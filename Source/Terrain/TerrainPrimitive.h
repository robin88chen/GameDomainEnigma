/*********************************************************************
 * \file   TerrainPrimitive.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   June 2023
 *********************************************************************/
#ifndef TERRAIN_PRIMITIVE_H
#define TERRAIN_PRIMITIVE_H

#include "Renderables/MeshPrimitive.h"

namespace Enigma::Terrain
{
    class TerrainPrimitive : public Renderables::MeshPrimitive
    {
        DECLARE_EN_RTTI;
    public:
        TerrainPrimitive(const Primitives::PrimitiveId& id);
        TerrainPrimitive(const Primitives::PrimitiveId& id, const Engine::GenericDto& dto, const std::shared_ptr<Geometries::GeometryRepository>& geometry_repository);
        virtual ~TerrainPrimitive() override;
        TerrainPrimitive(const TerrainPrimitive&) = delete;
        TerrainPrimitive& operator=(const TerrainPrimitive&) = delete;
        TerrainPrimitive(TerrainPrimitive&&) = delete;
        TerrainPrimitive& operator=(TerrainPrimitive&&) = delete;

        static std::shared_ptr<Primitives::Primitive> create(const Primitives::PrimitiveId& id);
        static std::shared_ptr<Primitives::Primitive> constitute(const Primitives::PrimitiveId& id, const Engine::GenericDto& dto, const std::shared_ptr<Geometries::GeometryRepository>& geometry_repository);
        //todo : implement these functions
        virtual std::shared_ptr<Primitives::PrimitiveAssembler> assembler() const override;
        virtual void assemble(const std::shared_ptr<Primitives::PrimitiveAssembler>& assembler) override;
    };
}

#endif // TERRAIN_PRIMITIVE_H
