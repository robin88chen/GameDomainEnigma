/*********************************************************************
 * \file   TerrainGeometry.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   June 2023
 *********************************************************************/
#ifndef TERRAIN_GEOMETRY_H
#define TERRAIN_GEOMETRY_H

#include "Geometries/TriangleList.h"
#include "MathLib/AlgebraBasicTypes.h"

namespace Enigma::Terrain
{
    class TerrainGeometry : public Geometries::TriangleList
    {
        DECLARE_EN_RTTI;
    public:
        TerrainGeometry(const Geometries::GeometryId& id);
        //TerrainGeometry(const Geometries::GeometryId& id, const Engine::GenericDto& o);
        virtual ~TerrainGeometry() override;
        TerrainGeometry(const TerrainGeometry&) = delete;
        TerrainGeometry& operator=(const TerrainGeometry&) = delete;
        TerrainGeometry(TerrainGeometry&&) = delete;
        TerrainGeometry& operator=(TerrainGeometry&&) = delete;

        //todo : implement later
        virtual std::shared_ptr<Geometries::GeometryAssembler> assembler() const override;
        virtual void assemble(const std::shared_ptr<Geometries::GeometryAssembler>& assembler) const override;
        virtual std::shared_ptr<Geometries::GeometryDisassembler> disassembler() override;
        virtual void disassemble(const std::shared_ptr<Geometries::GeometryDisassembler>& disassembler) override;

        void updateHeightMapToVertexMemory();
        void rangedUpdateHeightMapToVertexMemory(unsigned offset, unsigned count);
        void updateVertexNormals();
        void rangedUpdateVertexNormals(unsigned offset, unsigned count);

        MathLib::Dimension<float> getCellDimension() const;
        unsigned getNumRows() const { return m_numRows; }
        unsigned getNumCols() const { return m_numCols; }
        const MathLib::Vector3& getMinPosition() const { return m_minPosition; }
        const MathLib::Vector3& getMaxPosition() const { return m_maxPosition; }
        const float_buffer& getHeightMap() const { return m_heightMap; }
        float getHeight(unsigned x, unsigned z) const;
        void changeHeight(unsigned x, unsigned z, float new_height);

        unsigned convertVertexIndex(unsigned x, unsigned z) const;
        std::tuple<unsigned, unsigned> revertVertexIndex(unsigned index) const;

        std::tuple<unsigned, unsigned> locateCell(const MathLib::Vector3& position) const;

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
