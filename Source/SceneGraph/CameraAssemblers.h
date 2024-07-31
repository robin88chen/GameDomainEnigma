/*****************************************************************
 * \file   CameraAssemblers.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   June 2024
 ******************************************************************/
#ifndef CAMERA_ASSEMBLERS_H
#define CAMERA_ASSEMBLERS_H

#include "CameraDtos.h"
#include "SpatialId.h"

namespace Enigma::SceneGraph
{
    class Camera;
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

        std::shared_ptr<Camera> constitute();

    protected:
        Engine::GenericDto toGenericDto();

    protected:
        SpatialId m_id;
        CameraDto m_cameraDto;
        FrustumDto m_frustumDto;
    };
}

#endif // CAMERA_ASSEMBLERS_H
