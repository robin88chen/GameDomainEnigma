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

        CameraDtoHelper& eyePosition(const MathLib::Vector3& eye_position);
        CameraDtoHelper& lookAt(const MathLib::Vector3& look_at);
        CameraDtoHelper& upDirection(const MathLib::Vector3& up_direction);

        CameraDtoHelper& frustum(Frustum::ProjectionType projection_type);
        CameraDtoHelper& frustumFrontBackZ(float front_z, float back_z);
        CameraDtoHelper& frustumFov(float fov);
        CameraDtoHelper& frustumNearPlaneDimension(float width, float height);

        CameraDto toCameraDto();
        Engine::GenericDto toGenericDto();

    protected:
        CameraDto m_cameraDto;
        FrustumDto m_frustumDto;
    };

    class SpatialDtoHelper
    {
    public:
        SpatialDtoHelper(const std::string& name);

        SpatialDtoHelper& localTransform(const MathLib::Matrix4& local_transform);
        SpatialDtoHelper& worldTransform(const MathLib::Matrix4& world_transform);
        SpatialDtoHelper& modelBound(const Engine::BoundingVolume& model_bound);
        SpatialDtoHelper& topLevel(bool top_level);
        SpatialDtoHelper& cullingMode(Spatial::CullingMode cull_mode);
        SpatialDtoHelper& notifyFlags(Spatial::NotifyFlags notify_flags);
        SpatialDtoHelper& spatialFlags(Spatial::SpatialFlags spatial_flags);
        SpatialDtoHelper& graphDepth(unsigned graph_depth);

        Engine::GenericDto toGenericDto();

    private:
        SpatialDto m_dto;
        Engine::BoundingVolume m_modelBound;
    };

    class PawnDtoHelper
    {
    public:
        PawnDtoHelper(const std::string& name);

        PawnDtoHelper& factory(const Engine::FactoryDesc& factory);
        PawnDtoHelper& localTransform(const MathLib::Matrix4& local_transform);
        PawnDtoHelper& worldTransform(const MathLib::Matrix4& world_transform);
        PawnDtoHelper& modelBound(const Engine::BoundingVolume& model_bound);
        PawnDtoHelper& topLevel(bool top_level);
        PawnDtoHelper& cullingMode(Spatial::CullingMode cull_mode);
        PawnDtoHelper& notifyFlags(Spatial::NotifyFlags notify_flags);
        PawnDtoHelper& spatialFlags(Spatial::SpatialFlags spatial_flags);
        PawnDtoHelper& graphDepth(unsigned graph_depth);
        PawnDtoHelper& meshPrimitive(const Renderer::MeshPrimitiveDto& mesh_dto);

        PawnDto toPawnDto();
        Engine::GenericDto toGenericDto();

    private:
        PawnDto m_dto;
        Engine::BoundingVolume m_modelBound;
    };
}

#endif // SCENE_GRAPH_DTO_HELPER_H
