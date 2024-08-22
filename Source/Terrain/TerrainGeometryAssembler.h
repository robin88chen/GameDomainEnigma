/*********************************************************************
 * \file   TerrainGeometryAssembler.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   August 2024
 *********************************************************************/
#ifndef TERRAIN_GEOMETRY_ASSEMBLER_H
#define TERRAIN_GEOMETRY_ASSEMBLER_H

#include "Frameworks/ExtentTypesDefine.h"
#include "Geometries/TriangleListAssembler.h"
#include "GameEngine/GenericDto.h"
#include "GameEngine/BoundingVolume.h"
#include "MathLib/Vector2.h"
#include "MathLib/Vector3.h"
#include <optional>

namespace Enigma::Terrain
{
    class TerrainGeometryAssembler : public Geometries::TriangleListAssembler
    {
    public:
        TerrainGeometryAssembler(const Geometries::GeometryId& id);
        virtual ~TerrainGeometryAssembler() override = default;
        TerrainGeometryAssembler(const TerrainGeometryAssembler&) = default;
        TerrainGeometryAssembler(TerrainGeometryAssembler&&) = default;
        TerrainGeometryAssembler& operator=(const TerrainGeometryAssembler&) = default;
        TerrainGeometryAssembler& operator=(TerrainGeometryAssembler&&) = default;

        void numRows(unsigned rows) { m_numRows = rows; }
        void numCols(unsigned cols) { m_numCols = cols; }
        void minPosition(const MathLib::Vector3& min) { m_minPosition = min; }
        void maxPosition(const MathLib::Vector3& max) { m_maxPosition = max; }
        void minTextureCoordinate(const MathLib::Vector2& min) { m_minTextureCoordinate = min; }
        void maxTextureCoordinate(const MathLib::Vector2& max) { m_maxTextureCoordinate = max; }
        void heightMap(const std::optional<float_buffer>& height_map) { m_heightMap = height_map; }

        Engine::GenericDto assemble() override;

        void convertGeometryVertices();
        Engine::BoundingVolume calculateGeometryBounding();

    protected:
        unsigned calculateGeometryVertexCount() const;
        unsigned calculateGeometryIndexCount() const;

    protected:
        unsigned m_numRows;
        unsigned m_numCols;
        MathLib::Vector3 m_minPosition;
        MathLib::Vector3 m_maxPosition;
        MathLib::Vector2 m_minTextureCoordinate;
        MathLib::Vector2 m_maxTextureCoordinate;
        std::optional<float_buffer> m_heightMap;
    };
}

#endif // TERRAIN_GEOMETRY_ASSEMBLER_H
