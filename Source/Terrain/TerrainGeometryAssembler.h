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

    protected:
        void assembleNonVertexAttributes();

    protected:
        unsigned m_numRows;
        unsigned m_numCols;
        MathLib::Vector3 m_minPosition;
        MathLib::Vector3 m_maxPosition;
        MathLib::Vector2 m_minTextureCoordinate;
        MathLib::Vector2 m_maxTextureCoordinate;
        std::optional<float_buffer> m_heightMap;
    };
    class TerrainGeometryDisassembler : public Geometries::TriangleListDisassembler
    {
    public:
        TerrainGeometryDisassembler();
        virtual ~TerrainGeometryDisassembler() override = default;
        TerrainGeometryDisassembler(const TerrainGeometryDisassembler&) = default;
        TerrainGeometryDisassembler(TerrainGeometryDisassembler&&) = default;
        TerrainGeometryDisassembler& operator=(const TerrainGeometryDisassembler&) = default;
        TerrainGeometryDisassembler& operator=(TerrainGeometryDisassembler&&) = default;

        void disassemble(const Engine::GenericDto& dto) override;

        [[nodiscard]] unsigned numRows() const { return m_numRows; }
        [[nodiscard]] unsigned numCols() const { return m_numCols; }
        [[nodiscard]] const MathLib::Vector3& minPosition() const { return m_minPosition; }
        [[nodiscard]] const MathLib::Vector3& maxPosition() const { return m_maxPosition; }
        [[nodiscard]] const MathLib::Vector2& minTextureCoordinate() const { return m_minTextureCoordinate; }
        [[nodiscard]] const MathLib::Vector2& maxTextureCoordinate() const { return m_maxTextureCoordinate; }
        [[nodiscard]] const std::optional<float_buffer>& heightMap() const { return m_heightMap; }

    protected:
        void disassembleGeometryVertices();

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
