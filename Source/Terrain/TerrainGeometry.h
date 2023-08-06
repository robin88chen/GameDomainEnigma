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
