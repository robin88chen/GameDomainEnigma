#include "CameraFrustumDtos.h"
#include "Camera.h"
#include "MathLib/MathGlobal.h"

using namespace Enigma::Engine;
using namespace Enigma::SceneGraph;
using namespace Enigma::MathLib;

static std::string TOKEN_NAME = "Name";
static std::string TOKEN_HAND_SYSTEM = "HandSystem";
static std::string TOKEN_EYE_POSITION = "EyePosition";
static std::string TOKEN_LOOK_AT_DIR = "LookAtDir";
static std::string TOKEN_UP_VECTOR = "UpVector";
static std::string TOKEN_FRUSTUM = "Frustum";
static std::string TOKEN_PROJECTION_TYPE = "ProjectionType";
static std::string TOKEN_FOV = "Fov";
static std::string TOKEN_NEAR_PLANE_Z = "NearPlaneZ";
static std::string TOKEN_FAR_PLANE_Z = "FarPlaneZ";
static std::string TOKEN_ASPECT_RATIO = "AspectRatio";
static std::string TOKEN_NEAR_WIDTH = "NearWidth";
static std::string TOKEN_NEAR_HEIGHT = "NearHeight";

CameraDto::CameraDto() : m_handSys(GraphicCoordSys::LeftHand), m_factoryDesc(Camera::TYPE_RTTI.GetName())
{
}

CameraDto CameraDto::FromGenericDto(const Engine::GenericDto& dto)
{
    CameraDto camera;
    camera.TheFactoryDesc() = dto.GetRtti();
    if (const auto v = dto.TryGetValue<std::string>(TOKEN_NAME)) camera.Name() = v.value();
    if (const auto v = dto.TryGetValue<unsigned>(TOKEN_HAND_SYSTEM)) camera.HandSystem() = static_cast<GraphicCoordSys>(v.value());
    if (const auto v = dto.TryGetValue<Vector3>(TOKEN_EYE_POSITION)) camera.EyePosition() = v.value();
    if (const auto v = dto.TryGetValue<Vector3>(TOKEN_LOOK_AT_DIR)) camera.LookAtDirection() = v.value();
    if (const auto v = dto.TryGetValue<Vector3>(TOKEN_UP_VECTOR)) camera.UpVector() = v.value();
    if (const auto v = dto.TryGetValue<GenericDto>(TOKEN_FRUSTUM)) camera.Frustum() = v.value();
    return camera;
}

GenericDto CameraDto::ToGenericDto()
{
    GenericDto dto;
    dto.AddRtti(m_factoryDesc);
    dto.AddOrUpdate(TOKEN_NAME, m_name);
    dto.AddOrUpdate(TOKEN_HAND_SYSTEM, static_cast<unsigned>(m_handSys));
    dto.AddOrUpdate(TOKEN_EYE_POSITION, m_eyePosition);
    dto.AddOrUpdate(TOKEN_LOOK_AT_DIR, m_lookAtDir);
    dto.AddOrUpdate(TOKEN_UP_VECTOR, m_up);
    dto.AddOrUpdate(TOKEN_FRUSTUM, m_frustumDto);
    return dto;
}

FrustumDto::FrustumDto() : m_handSys(GraphicCoordSys::LeftHand), m_projectionType(Frustum::ProjectionType::Perspective),
m_fov(MathLib::Math::PI / 4.0f), m_nearPlaneZ(0.1f), m_farPlaneZ(100.0f), m_aspectRatio(4.0f / 3.0f), m_nearWidth(40.0f), m_nearHeight(30.0f), m_factoryDesc(Frustum::TYPE_RTTI.GetName())
{
}

FrustumDto FrustumDto::FromGenericDto(const Engine::GenericDto& dto)
{
    FrustumDto frustum;
    frustum.TheFactoryDesc() = dto.GetRtti();
    if (const auto v = dto.TryGetValue<unsigned>(TOKEN_HAND_SYSTEM)) frustum.HandSystem() = static_cast<GraphicCoordSys>(v.value());
    if (const auto v = dto.TryGetValue<unsigned>(TOKEN_PROJECTION_TYPE)) frustum.ProjectionType() = static_cast<Frustum::ProjectionType>(v.value());
    if (const auto v = dto.TryGetValue<float>(TOKEN_FOV)) frustum.Fov() = v.value();
    if (const auto v = dto.TryGetValue<float>(TOKEN_NEAR_PLANE_Z)) frustum.NearPlaneZ() = v.value();
    if (const auto v = dto.TryGetValue<float>(TOKEN_FAR_PLANE_Z)) frustum.FarPlaneZ() = v.value();
    if (const auto v = dto.TryGetValue<float>(TOKEN_ASPECT_RATIO)) frustum.AspectRatio() = v.value();
    if (const auto v = dto.TryGetValue<float>(TOKEN_NEAR_WIDTH)) frustum.NearWidth() = v.value();
    if (const auto v = dto.TryGetValue<float>(TOKEN_NEAR_HEIGHT)) frustum.NearHeight() = v.value();
    return frustum;
}

GenericDto FrustumDto::ToGenericDto()
{
    GenericDto dto;
    dto.AddRtti(m_factoryDesc);
    dto.AddOrUpdate(TOKEN_HAND_SYSTEM, static_cast<unsigned>(m_handSys));
    dto.AddOrUpdate(TOKEN_PROJECTION_TYPE, static_cast<unsigned>(m_projectionType));
    dto.AddOrUpdate(TOKEN_FOV, m_fov);
    dto.AddOrUpdate(TOKEN_NEAR_PLANE_Z, m_nearPlaneZ);
    dto.AddOrUpdate(TOKEN_FAR_PLANE_Z, m_farPlaneZ);
    dto.AddOrUpdate(TOKEN_ASPECT_RATIO, m_aspectRatio);
    dto.AddOrUpdate(TOKEN_NEAR_WIDTH, m_nearWidth);
    dto.AddOrUpdate(TOKEN_NEAR_HEIGHT, m_nearHeight);
    return dto;
}
