/*********************************************************************
 * \file   SceneGraphAssemblers.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   May 2023
 *********************************************************************/
#ifndef SCENE_GRAPH_ASSEMBLERS_H
#define SCENE_GRAPH_ASSEMBLERS_H

#include <string>
#include "CameraFrustumDtos.h"
#include "GameEngine/BoundingVolume.h"
#include "SceneGraphDtos.h"
#include "SceneGraph/Spatial.h"
#include "SceneGraphPersistenceLevel.h"

namespace Enigma::SceneGraph
{
    class Camera;
    class Pawn;
    class CameraAssembler
    {
    public:
        CameraAssembler(const SpatialId& id);

        CameraAssembler& eyePosition(const MathLib::Vector3& eye_position);
        CameraAssembler& lookAt(const MathLib::Vector3& look_at);
        CameraAssembler& upDirection(const MathLib::Vector3& up_direction);

        CameraAssembler& frustum(Frustum::ProjectionType projection_type);
        CameraAssembler& frustumFrontBackZ(float front_z, float back_z);
        CameraAssembler& frustumFov(float fov);
        CameraAssembler& frustumNearPlaneDimension(float width, float height);

        CameraAssembler& asNative(const std::string& file_at_path);
        CameraDto toCameraDto();

        std::shared_ptr<Camera> constitute(PersistenceLevel persistence_level);

    protected:
        Engine::GenericDto toGenericDto();

    protected:
        SpatialId m_id;
        CameraDto m_cameraDto;
        FrustumDto m_frustumDto;
    };

    class SpatialAssembler
    {
    public:
        SpatialAssembler(const SpatialId& id);

        SpatialAssembler& localTransform(const MathLib::Matrix4& local_transform);
        SpatialAssembler& worldTransform(const MathLib::Matrix4& world_transform);
        SpatialAssembler& modelBound(const Engine::BoundingVolume& model_bound);
        SpatialAssembler& topLevel(bool top_level);
        SpatialAssembler& cullingMode(Spatial::CullingMode cull_mode);
        SpatialAssembler& notifyFlags(Spatial::NotifyFlags notify_flags);
        SpatialAssembler& spatialFlags(Spatial::SpatialFlags spatial_flags);
        SpatialAssembler& graphDepth(unsigned graph_depth);

        Engine::GenericDto toGenericDto();

    private:
        SpatialId m_id;
        SpatialDto m_dto;
        Engine::BoundingVolume m_modelBound;
    };

    class PawnAssembler
    {
    public:
        PawnAssembler(const SpatialId& id);

        PawnAssembler& factory(const Engine::FactoryDesc& factory);
        PawnAssembler& localTransform(const MathLib::Matrix4& local_transform);
        PawnAssembler& worldTransform(const MathLib::Matrix4& world_transform);
        PawnAssembler& modelBound(const Engine::BoundingVolume& model_bound);
        PawnAssembler& topLevel(bool top_level);
        PawnAssembler& cullingMode(Spatial::CullingMode cull_mode);
        PawnAssembler& notifyFlags(Spatial::NotifyFlags notify_flags);
        PawnAssembler& spatialFlags(Spatial::SpatialFlags spatial_flags);
        PawnAssembler& graphDepth(unsigned graph_depth);
        PawnAssembler& primitive(const Primitives::PrimitiveId& primitive_id);
        PawnAssembler& asNative(const std::string& file_at_path);

        PawnDto toPawnDto();
        std::shared_ptr<Pawn> constitute(PersistenceLevel persistence_level);

    protected:
        Engine::GenericDto toGenericDto();

    private:
        SpatialId m_id;
        PawnDto m_dto;
        Engine::BoundingVolume m_modelBound;
    };
}

#endif // SCENE_GRAPH_ASSEMBLERS_H
