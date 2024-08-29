/*********************************************************************
 * \file   TerrainAssemblers.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   July 2023
 *********************************************************************/
#ifndef TERRAIN_ASSEMBLERS_H
#define TERRAIN_ASSEMBLERS_H

#include "TerrainGeometryDto.h"
#include "TerrainPawnDto.h"
#include "GameEngine/BoundingVolume.h"
#include "TerrainPrimitiveDto.h"
#include "Frameworks/ExtentTypesDefine.h"
#include "SceneGraph/SceneGraphAssemblers.h"

namespace Enigma::Terrain
{
    /*class TerrainGeometryAssembler
    {
    public:
        TerrainGeometryAssembler(const Geometries::GeometryId& id);

        TerrainGeometryAssembler& numRows(unsigned num_rows);
        TerrainGeometryAssembler& numCols(unsigned num_cols);
        TerrainGeometryAssembler& minPosition(const MathLib::Vector3& min_position);
        TerrainGeometryAssembler& maxPosition(const MathLib::Vector3& max_position);
        TerrainGeometryAssembler& minTextureCoordinate(const MathLib::Vector2& min_texture_coordinate);
        TerrainGeometryAssembler& maxTextureCoordinate(const MathLib::Vector2& max_texture_coordinate);
        TerrainGeometryAssembler& heightMap(const float_buffer& height_map);
        TerrainGeometryAssembler& asAsset(const std::string& name, const std::string& filename, const std::string& path_id);

        Engine::GenericDto toGenericDto() const;
        //TerrainGeometryDto dto() const;

    private:
        Geometries::GeometryId m_id;
        //TerrainGeometryDto m_dto;
        Engine::BoundingVolume m_geometryBound;
    };*/

    /*class TerrainPrimitiveAssembler
    {
    public:
        TerrainPrimitiveAssembler(const Primitives::PrimitiveId& id);

        Renderables::MeshPrimitiveAssembler& meshPrimitive();

        TerrainPrimitiveAssembler& asNative(const std::string& file_at_path);

        TerrainPrimitiveDto dto() const;
        Engine::GenericDto toGenericDto() const;

    private:
        Primitives::PrimitiveId m_id;
        Renderables::MeshPrimitiveAssembler m_meshPrimitiveAssembler;
        TerrainPrimitiveDto m_dto;
    };*/

    class TerrainPawnAssembler
    {
    public:
        TerrainPawnAssembler(const SceneGraph::SpatialId& id);

        const SceneGraph::SpatialId& id() const { return m_id; }

        SceneGraph::PawnAssembler& pawn();
        TerrainPawnAssembler& factory(const Engine::FactoryDesc& factory);
        TerrainPawnAssembler& asNative(const std::string& file_at_path);

        TerrainPawnDto dto() const;
        Engine::GenericDto toGenericDto() const;

    private:
        SceneGraph::SpatialId m_id;
        SceneGraph::PawnAssembler m_pawnAssembler;
        TerrainPawnDto m_dto;
    };
}

#endif // TERRAIN_ASSEMBLERS_H
