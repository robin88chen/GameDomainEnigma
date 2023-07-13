/*********************************************************************
 * \file   TerrainDtoHelper.h
 * \brief  
 * 
 * \author Lancelot 'Robin' Chen
 * \date   July 2023
 *********************************************************************/
#ifndef TERRAIN_DTO_HELPER_H
#define TERRAIN_DTO_HELPER_H

#include "TerrainPawnDto.h"
#include "GameEngine/BoundingVolume.h"
#include "Renderer/RenderablePrimitiveDtos.h"
#include "TerrainPrimitiveDto.h"

namespace Enigma::Terrain
{
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

        Engine::GenericDto ToGenericDto() const;

    private:
        TerrainPawnDto m_dto;
        Engine::BoundingVolume m_modelBound;
    };
}

#endif // TERRAIN_DTO_HELPER_H
