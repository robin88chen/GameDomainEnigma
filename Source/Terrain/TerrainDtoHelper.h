/*********************************************************************
 * \file   TerrainDtoHelper.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   July 2023
 *********************************************************************/
#ifndef TERRAIN_DTO_HELPER_H
#define TERRAIN_DTO_HELPER_H

#include "TerrainGeometryDto.h"
#include "TerrainPawnDto.h"
#include "GameEngine/BoundingVolume.h"
#include "Renderer/RenderablePrimitiveDtos.h"
#include "TerrainPrimitiveDto.h"
#include "Frameworks/ExtentTypesDefine.h"

namespace Enigma::Terrain
{
    class TerrainGeometryDtoHelper
    {
    public:
        TerrainGeometryDtoHelper(const std::string& name);

        TerrainGeometryDtoHelper& numRows(unsigned num_rows);
        TerrainGeometryDtoHelper& numCols(unsigned num_cols);
        TerrainGeometryDtoHelper& minPosition(const MathLib::Vector3& min_position);
        TerrainGeometryDtoHelper& maxPosition(const MathLib::Vector3& max_position);
        TerrainGeometryDtoHelper& minTextureCoordinate(const MathLib::Vector2& min_texture_coordinate);
        TerrainGeometryDtoHelper& maxTextureCoordinate(const MathLib::Vector2& max_texture_coordinate);
        TerrainGeometryDtoHelper& heightMap(const float_buffer& height_map);

        Engine::GenericDto toGenericDto() const;
        TerrainGeometryDto toDto() const;

    private:
        TerrainGeometryDto m_dto;
        Engine::BoundingVolume m_geometryBound;
    };

    class TerrainPawnDtoHelper
    {
    public:
        TerrainPawnDtoHelper(const std::string& name);

        TerrainPawnDtoHelper& factory(const Engine::FactoryDesc& factory);
        TerrainPawnDtoHelper& localTransform(const MathLib::Matrix4& local_transform);
        TerrainPawnDtoHelper& worldTransform(const MathLib::Matrix4& world_transform);
        TerrainPawnDtoHelper& modelBound(const Engine::BoundingVolume& model_bound);
        TerrainPawnDtoHelper& topLevel(bool top_level);
        TerrainPawnDtoHelper& cullingMode(SceneGraph::Spatial::CullingMode cull_mode);
        TerrainPawnDtoHelper& notifyFlags(SceneGraph::Spatial::NotifyFlags notify_flags);
        TerrainPawnDtoHelper& spatialFlags(SceneGraph::Spatial::SpatialFlags spatial_flags);
        TerrainPawnDtoHelper& graphDepth(unsigned graph_depth);
        TerrainPawnDtoHelper& terrainPrimitive(const TerrainPrimitiveDto& terrain_primitive_dto);

        Engine::GenericDto toGenericDto() const;

    private:
        TerrainPawnDto m_dto;
        Engine::BoundingVolume m_modelBound;
    };
}

#endif // TERRAIN_DTO_HELPER_H
