/*********************************************************************
 * \file   TerrainGeometry.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   June 2023
 *********************************************************************/
#ifndef TERRAIN_GEOMETRY_H
#define TERRAIN_GEOMETRY_H

#include "GameEngine/TriangleList.h"
#include "MathLib/AlgebraBasicTypes.h"

namespace Enigma::Terrain
{
    class TerrainGeometry : public Engine::TriangleList
    {
        DECLARE_EN_RTTI;
    public:
        TerrainGeometry(const std::string& name);
        TerrainGeometry(const Engine::GenericDto& o);
        virtual ~TerrainGeometry() override;
        TerrainGeometry(const TerrainGeometry&) = delete;
        TerrainGeometry& operator=(const TerrainGeometry&) = delete;
        TerrainGeometry(TerrainGeometry&&) = delete;
        TerrainGeometry& operator=(TerrainGeometry&&) = delete;

        virtual Engine::GenericDto SerializeDto() const override;

        void UpdateHeightMapToVertexMemory();
        void RangedUpdateHeightMapToVertexMemory(unsigned offset, unsigned count);

        MathLib::Dimension<float> GetCellDimension() const;
        unsigned GetNumRows() const { return m_numRows; }
        unsigned GetNumCols() const { return m_numCols; }
        const MathLib::Vector3& GetMinPosition() const { return m_minPosition; }
        const MathLib::Vector3& GetMaxPosition() const { return m_maxPosition; }
        const float_buffer& GetHeightMap() const { return m_heightMap; }
        void ChangeHeight(unsigned idx, float new_height);
    protected:
        unsigned m_numRows;
        unsigned m_numCols;
        MathLib::Vector3 m_minPosition;
        MathLib::Vector3 m_maxPosition;
        MathLib::Vector2 m_minTextureCoordinate;
        MathLib::Vector2 m_maxTextureCoordinate;
        float_buffer m_heightMap;
    };
}

#endif // TERRAIN_GEOMETRY_H
