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

        virtual Engine::GenericDto serializeDto() const override;

        void UpdateHeightMapToVertexMemory();
        void RangedUpdateHeightMapToVertexMemory(unsigned offset, unsigned count);
        void UpdateVertexNormals();
        void RangedUpdateVertexNormals(unsigned offset, unsigned count);

        MathLib::Dimension<float> GetCellDimension() const;
        unsigned GetNumRows() const { return m_numRows; }
        unsigned GetNumCols() const { return m_numCols; }
        const MathLib::Vector3& GetMinPosition() const { return m_minPosition; }
        const MathLib::Vector3& GetMaxPosition() const { return m_maxPosition; }
        const float_buffer& GetHeightMap() const { return m_heightMap; }
        float GetHeight(unsigned x, unsigned z) const;
        void ChangeHeight(unsigned x, unsigned z, float new_height);

        unsigned ConvertVertexIndex(unsigned x, unsigned z) const;
        std::tuple<unsigned, unsigned> RevertVertexIndex(unsigned index) const;

        std::tuple<unsigned, unsigned> LocateCell(const MathLib::Vector3& position) const;

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
