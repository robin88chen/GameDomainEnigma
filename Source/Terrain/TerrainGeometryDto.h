/*********************************************************************
 * \file   TerrainGeometryDto.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   June 2023
 *********************************************************************/
#ifndef TERRAIN_GEOMETRY_DTO_H
#define TERRAIN_GEOMETRY_DTO_H

#include "GameEngine/GeometryDataDto.h"
#include "Frameworks/ExtentTypesDefine.h"
#include "GameEngine/BoundingVolume.h"

namespace Enigma::Terrain
{
    class TerrainGeometryDto : public Engine::TriangleListDto
    {
    public:
        TerrainGeometryDto();
        TerrainGeometryDto(const Engine::TriangleListDto& triangle_dto);

        static TerrainGeometryDto FromGenericDto(const Engine::GenericDto& dto);
        Engine::GenericDto ToGenericDto();

        [[nodiscard]] unsigned NumRows() const { return m_numRows; }
        unsigned& NumRows() { return m_numRows; }
        [[nodiscard]] unsigned NumCols() const { return m_numCols; }
        unsigned& NumCols() { return m_numCols; }
        [[nodiscard]] MathLib::Vector3 MinPosition() const { return m_minPosition; }
        MathLib::Vector3& MinPosition() { return m_minPosition; }
        [[nodiscard]] MathLib::Vector3 MaxPosition() const { return m_maxPosition; }
        MathLib::Vector3& MaxPosition() { return m_maxPosition; }
        [[nodiscard]] MathLib::Vector2 MinTextureCoordinate() const { return m_minTextureCoordinate; }
        MathLib::Vector2& MinTextureCoordinate() { return m_minTextureCoordinate; }
        [[nodiscard]] MathLib::Vector2 MaxTextureCoordinate() const { return m_maxTextureCoordinate; }
        MathLib::Vector2& MaxTextureCoordinate() { return m_maxTextureCoordinate; }

        [[nodiscard]] std::optional<float_buffer> HeightMap() const { return m_heightMap; }
        std::optional<float_buffer>& HeightMap() { return m_heightMap; }

        void ConvertGeometryVertices();

    protected:
        unsigned CalculateGeometryVertexCount() const;
        unsigned CalculateGeometryIndexCount() const;
        Engine::BoundingVolume CalculateGeometryBounding();

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
