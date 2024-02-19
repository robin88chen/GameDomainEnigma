/*********************************************************************
 * \file   SceneGraphHelper.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   February 2024
 *********************************************************************/
#ifndef SCENE_GRAPH_HELPER_H
#define SCENE_GRAPH_HELPER_H

#include "SceneGraphDtoHelper.h"
#include "SceneGraphPersistenceLevel.h"

namespace Enigma::SceneGraph
{
    class Camera;
    class Pawn;
    class CameraHelper
    {
    public:
        CameraHelper(const SpatialId& id);

        CameraHelper& eyePosition(const MathLib::Vector3& eye_position);
        CameraHelper& lookAt(const MathLib::Vector3& look_at);
        CameraHelper& upDirection(const MathLib::Vector3& up_direction);

        CameraHelper& frustum(Frustum::ProjectionType projection_type);
        CameraHelper& frustumFrontBackZ(float front_z, float back_z);
        CameraHelper& frustumFov(float fov);
        CameraHelper& frustumNearPlaneDimension(float width, float height);
        CameraHelper& asNative(const std::string& file_at_path);

        std::shared_ptr<Camera> constitute(PersistenceLevel persistence_level);

    protected:
        SpatialId m_id;
        CameraDtoHelper m_dto;
    };

    class PawnHelper
    {
    public:
        PawnHelper(const SpatialId& id);

        PawnHelper& factory(const Engine::FactoryDesc& factory);
        PawnHelper& localTransform(const MathLib::Matrix4& local_transform);
        PawnHelper& worldTransform(const MathLib::Matrix4& world_transform);
        PawnHelper& modelBound(const Engine::BoundingVolume& model_bound);
        PawnHelper& topLevel(bool top_level);
        PawnHelper& cullingMode(Spatial::CullingMode cull_mode);
        PawnHelper& notifyFlags(Spatial::NotifyFlags notify_flags);
        PawnHelper& spatialFlags(Spatial::SpatialFlags spatial_flags);
        PawnHelper& graphDepth(unsigned graph_depth);
        PawnHelper& primitive(const Primitives::PrimitiveId& primitive_id);
        PawnHelper& asNative(const std::string& file_at_path);

        std::shared_ptr<Pawn> constitute(PersistenceLevel persistence_level);

    protected:
        SpatialId m_id;
        PawnDtoHelper m_dto;
    };
}

#endif // SCENE_GRAPH_HELPER_H
