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

        TerrainGeometryDtoHelper& NumRows(unsigned num_rows);
        TerrainGeometryDtoHelper& NumCols(unsigned num_cols);
        TerrainGeometryDtoHelper& MinPosition(const MathLib::Vector3& min_position);
        TerrainGeometryDtoHelper& MaxPosition(const MathLib::Vector3& max_position);
        TerrainGeometryDtoHelper& MinTextureCoordinate(const MathLib::Vector2& min_texture_coordinate);
        TerrainGeometryDtoHelper& MaxTextureCoordinate(const MathLib::Vector2& max_texture_coordinate);
        TerrainGeometryDtoHelper& HeightMap(const float_buffer& height_map);

        Engine::GenericDto toGenericDto() const;
        TerrainGeometryDto ToDto() const;

    private:
        TerrainGeometryDto m_dto;
        Engine::BoundingVolume m_geometryBound;
    };

    class TerrainPawnDtoHelper
    {
    public:
        TerrainPawnDtoHelper(const std::string& name);

        TerrainPawnDtoHelper& Factory(const Engine::FactoryDesc& factory);
        TerrainPawnDtoHelper& LocalTransform(const MathLib::Matrix4& local_transform);
        TerrainPawnDtoHelper& WorldTransform(const MathLib::Matrix4& world_transform);
        TerrainPawnDtoHelper& ModelBound(const Engine::BoundingVolume& model_bound);
        TerrainPawnDtoHelper& TopLevel(bool top_level);
        TerrainPawnDtoHelper& CullingMode(SceneGraph::Spatial::CullingMode cull_mode);
        TerrainPawnDtoHelper& NotifyFlags(SceneGraph::Spatial::NotifyFlags notify_flags);
        TerrainPawnDtoHelper& SpatialFlags(SceneGraph::Spatial::SpatialFlags spatial_flags);
        TerrainPawnDtoHelper& GraphDepth(unsigned graph_depth);
        TerrainPawnDtoHelper& TerrainPrimitive(const TerrainPrimitiveDto& terrain_primitive_dto);

        Engine::GenericDto toGenericDto() const;

    private:
        TerrainPawnDto m_dto;
        Engine::BoundingVolume m_modelBound;
    };
}

#endif // TERRAIN_DTO_HELPER_H
