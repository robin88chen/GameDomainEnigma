﻿/*********************************************************************
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
        virtual ~TerrainPrimitive() override;
        TerrainPrimitive(const TerrainPrimitive&) = delete;
        TerrainPrimitive& operator=(const TerrainPrimitive&) = delete;
        TerrainPrimitive(TerrainPrimitive&&) = delete;
        TerrainPrimitive& operator=(TerrainPrimitive&&) = delete;

        static std::shared_ptr<TerrainPrimitive> create(const Primitives::PrimitiveId& id);

        virtual std::shared_ptr<Primitives::PrimitiveAssembler> assembler() const override;
        virtual std::shared_ptr<Primitives::PrimitiveDisassembler> disassembler() const override;
    };
}

#endif // TERRAIN_PRIMITIVE_H
