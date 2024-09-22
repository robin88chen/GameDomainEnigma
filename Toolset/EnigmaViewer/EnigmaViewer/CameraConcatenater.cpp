#include "CameraConcatenater.h"

using namespace Enigma::SceneGraph;

CameraConcatenater::CameraConcatenater(const SpatialId& id)
{
    m_cameraAssembler = std::make_shared<CameraAssembler>(id);
}

CameraConcatenater& CameraConcatenater::eyePosition(const Enigma::MathLib::Vector3& eye_position)
{
    m_cameraAssembler->eyePosition(eye_position);
    return *this;
}

CameraConcatenater& CameraConcatenater::lookAt(const Enigma::MathLib::Vector3& look_at)
{
    m_cameraAssembler->lookAt(look_at);
    return *this;
}

CameraConcatenater& CameraConcatenater::upVector(const Enigma::MathLib::Vector3& up_vector)
{
    m_cameraAssembler->upDirection(up_vector);
    return *this;
}

CameraConcatenater& CameraConcatenater::frustum(Frustum::ProjectionType projection)
{
    m_frustumAssembler = std::make_shared<FrustumAssembler>(projection);
    m_frustumAssembler->projectionType(projection);
    m_cameraAssembler->frustum(m_frustumAssembler);
    return *this;
}

CameraConcatenater& CameraConcatenater::frustumFov(const Enigma::MathLib::Radian& fov)
{
    m_frustumAssembler->fov(fov);
    return *this;
}

CameraConcatenater& CameraConcatenater::frustumNearPlaneDimension(float width, float height)
{
    m_frustumAssembler->nearPlaneDimension(width, height);
    return *this;
}

CameraConcatenater& CameraConcatenater::frustumFrontBackZ(float front_z, float back_z)
{
    m_frustumAssembler->frontBackZ(front_z, back_z);
    return *this;
}

CameraConcatenater& CameraConcatenater::asNative(const std::string& file_at_path)
{
    m_cameraAssembler->asNative(file_at_path);
    return *this;
}

Enigma::Engine::GenericDto CameraConcatenater::assemble()
{
    return m_cameraAssembler->assemble();
}
