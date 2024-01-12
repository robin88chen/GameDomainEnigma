/*********************************************************************
 * \file   TerrainGeometryDto.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   June 2023
 *********************************************************************/
#ifndef TERRAIN_GEOMETRY_DTO_H
#define TERRAIN_GEOMETRY_DTO_H

#include "Geometries/GeometryDataDto.h"
#include "Frameworks/ExtentTypesDefine.h"
#include "GameEngine/BoundingVolume.h"

namespace Enigma::Terrain
{
    class TerrainGeometryDto : public Geometries::TriangleListDto
    {
    public:
        TerrainGeometryDto();
        TerrainGeometryDto(const Geometries::TriangleListDto& triangle_dto);

        static TerrainGeometryDto fromGenericDto(const Engine::GenericDto& dto);
        Engine::GenericDto toGenericDto() const;

        [[nodiscard]] unsigned numRows() const { return m_numRows; }
        unsigned& numRows() { return m_numRows; }
        [[nodiscard]] unsigned numCols() const { return m_numCols; }
        unsigned& numCols() { return m_numCols; }
        [[nodiscard]] MathLib::Vector3 minPosition() const { return m_minPosition; }
        MathLib::Vector3& minPosition() { return m_minPosition; }
        [[nodiscard]] MathLib::Vector3 maxPosition() const { return m_maxPosition; }
        MathLib::Vector3& maxPosition() { return m_maxPosition; }
        [[nodiscard]] MathLib::Vector2 minTextureCoordinate() const { return m_minTextureCoordinate; }
        MathLib::Vector2& minTextureCoordinate() { return m_minTextureCoordinate; }
        [[nodiscard]] MathLib::Vector2 maxTextureCoordinate() const { return m_maxTextureCoordinate; }
        MathLib::Vector2& maxTextureCoordinate() { return m_maxTextureCoordinate; }

        [[nodiscard]] std::optional<float_buffer> heightMap() const { return m_heightMap; }
        std::optional<float_buffer>& heightMap() { return m_heightMap; }

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

#endif // TERRAIN_GEOMETRY_DTO_H
