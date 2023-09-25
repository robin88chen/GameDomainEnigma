/*********************************************************************
 * \file   SceneGraphDtoHelper.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   May 2023
 *********************************************************************/
#ifndef SCENE_GRAPH_DTO_HELPER_H
#define SCENE_GRAPH_DTO_HELPER_H

#include <string>
#include "CameraFrustumDtos.h"
#include "GameEngine/BoundingVolume.h"
#include "SceneGraphDtos.h"

namespace Enigma::SceneGraph
{
    class CameraDtoHelper
    {
    public:
        CameraDtoHelper(const std::string& name);

        CameraDtoHelper& EyePosition(const MathLib::Vector3& eye_position);
        CameraDtoHelper& LookAt(const MathLib::Vector3& look_at);
        CameraDtoHelper& UpDirection(const MathLib::Vector3& up_direction);

        CameraDtoHelper& Frustum(Frustum::ProjectionType projection_type);
        CameraDtoHelper& FrustumFrontBackZ(float front_z, float back_z);
        CameraDtoHelper& FrustumFov(float fov);
        CameraDtoHelper& FrustumNearPlaneDimension(float width, float height);

        CameraDto ToCameraDto();
        Engine::GenericDto ToGenericDto();

    protected:
        CameraDto m_cameraDto;
        FrustumDto m_frustumDto;
    };

    class SpatialDtoHelper
    {
    public:
        SpatialDtoHelper(const std::string& name);

        SpatialDtoHelper& LocalTransform(const MathLib::Matrix4& local_transform);
        SpatialDtoHelper& WorldTransform(const MathLib::Matrix4& world_transform);
        SpatialDtoHelper& ModelBound(const Engine::BoundingVolume& model_bound);
        SpatialDtoHelper& TopLevel(bool top_level);
        SpatialDtoHelper& CullingMode(Spatial::CullingMode cull_mode);
        SpatialDtoHelper& NotifyFlags(Spatial::NotifyFlags notify_flags);
        SpatialDtoHelper& SpatialFlags(Spatial::SpatialFlags spatial_flags);
        SpatialDtoHelper& GraphDepth(unsigned graph_depth);

        Engine::GenericDto ToGenericDto();

    private:
        SpatialDto m_dto;
        Engine::BoundingVolume m_modelBound;
    };

    class PawnDtoHelper
    {
    public:
        PawnDtoHelper(const std::string& name);

        PawnDtoHelper& Factory(const Engine::FactoryDesc& factory);
        PawnDtoHelper& LocalTransform(const MathLib::Matrix4& local_transform);
        PawnDtoHelper& WorldTransform(const MathLib::Matrix4& world_transform);
        PawnDtoHelper& ModelBound(const Engine::BoundingVolume& model_bound);
        PawnDtoHelper& TopLevel(bool top_level);
        PawnDtoHelper& CullingMode(Spatial::CullingMode cull_mode);
        PawnDtoHelper& NotifyFlags(Spatial::NotifyFlags notify_flags);
        PawnDtoHelper& SpatialFlags(Spatial::SpatialFlags spatial_flags);
        PawnDtoHelper& GraphDepth(unsigned graph_depth);
        PawnDtoHelper& MeshPrimitive(const Renderer::MeshPrimitiveDto& mesh_dto);

        PawnDto ToPawnDto();
        Engine::GenericDto ToGenericDto();

    private:
        PawnDto m_dto;
        Engine::BoundingVolume m_modelBound;
    };
}

#endif // SCENE_GRAPH_DTO_HELPER_H
