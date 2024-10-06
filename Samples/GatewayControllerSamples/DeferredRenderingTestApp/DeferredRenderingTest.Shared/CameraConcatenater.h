/*********************************************************************
 * \file   CameraConcatenater.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   September 2024
 *********************************************************************/
#ifndef CAMERA_CONCATENATER_H
#define CAMERA_CONCATENATER_H

#include "SceneGraph/SpatialId.h"
#include "SceneGraph/CameraAssembler.h"
#include "SceneGraph/Frustum.h"
#include "SceneGraph/FrustumAssembler.h"

class CameraConcatenater
{
public:
    CameraConcatenater(const Enigma::SceneGraph::SpatialId& id);

    CameraConcatenater& eyePosition(const Enigma::MathLib::Vector3& eye_position);
    CameraConcatenater& lookAt(const Enigma::MathLib::Vector3& look_at);
    CameraConcatenater& upVector(const Enigma::MathLib::Vector3& up_vector);
    CameraConcatenater& frustum(Enigma::SceneGraph::Frustum::ProjectionType projection);
    CameraConcatenater& frustumFov(const Enigma::MathLib::Radian& fov);
    CameraConcatenater& frustumNearPlaneDimension(float width, float height);
    CameraConcatenater& frustumFrontBackZ(float front_z, float back_z);
    CameraConcatenater& asNative(const std::string& file_at_path);

    Enigma::Engine::GenericDto assemble();

private:
    std::shared_ptr<Enigma::SceneGraph::CameraAssembler> m_cameraAssembler;
    std::shared_ptr<Enigma::SceneGraph::FrustumAssembler> m_frustumAssembler;
};

#endif // CAMERA_CONCATENATER_H
