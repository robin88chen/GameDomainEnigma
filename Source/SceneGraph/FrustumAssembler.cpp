#include "FrustumAssembler.h"
#include "Frustum.h"
#include "GameEngine/GenericDto.h"
#include <cassert>

using namespace Enigma::SceneGraph;

static std::string TOKEN_HAND_SYSTEM = "HandSystem";
static std::string TOKEN_PROJECTION_TYPE = "ProjectionType";
static std::string TOKEN_FOV = "Fov";
static std::string TOKEN_NEAR_PLANE_Z = "NearPlaneZ";
static std::string TOKEN_FAR_PLANE_Z = "FarPlaneZ";
static std::string TOKEN_ASPECT_RATIO = "AspectRatio";
static std::string TOKEN_NEAR_WIDTH = "NearWidth";
static std::string TOKEN_NEAR_HEIGHT = "NearHeight";

FrustumAssembler::FrustumAssembler(Frustum::ProjectionType projection_type) : m_factoryDesc(Frustum::TYPE_RTTI)
{
    m_projectionType = projection_type;
    m_handSys = GraphicCoordSys::LeftHand;
}

void FrustumAssembler::frontBackZ(float front_z, float back_z)
{
    assert(front_z < back_z);
    m_nearPlaneZ = front_z;
    m_farPlaneZ = back_z;
}

void FrustumAssembler::fov(MathLib::Radian fov)
{
    assert(fov.value() > 0.0f);
    m_fov = fov;
}

void FrustumAssembler::nearPlaneDimension(float width, float height)
{
    assert(width > 0.0f && height > 0.0f);
    m_nearWidth = width;
    m_nearHeight = height;
    m_aspectRatio = width / height;
}

Enigma::Engine::GenericDto FrustumAssembler::assemble()
{
    Engine::GenericDto dto;
    dto.addRtti(m_factoryDesc);
    dto.addOrUpdate(TOKEN_HAND_SYSTEM, static_cast<unsigned>(m_handSys));
    dto.addOrUpdate(TOKEN_PROJECTION_TYPE, static_cast<unsigned>(m_projectionType));
    dto.addOrUpdate(TOKEN_FOV, m_fov.value());
    dto.addOrUpdate(TOKEN_NEAR_PLANE_Z, m_nearPlaneZ);
    dto.addOrUpdate(TOKEN_FAR_PLANE_Z, m_farPlaneZ);
    dto.addOrUpdate(TOKEN_ASPECT_RATIO, m_aspectRatio);
    dto.addOrUpdate(TOKEN_NEAR_WIDTH, m_nearWidth);
    dto.addOrUpdate(TOKEN_NEAR_HEIGHT, m_nearHeight);
    return dto;
}

FrustumDisassembler::FrustumDisassembler() : m_factoryDesc(Frustum::TYPE_RTTI)
{
}

FrustumDisassembler::FrustumDisassembler(const Engine::GenericDto& dto) : m_factoryDesc(Frustum::TYPE_RTTI)
{
    disassemble(dto);
}

void FrustumDisassembler::disassemble(const Engine::GenericDto& dto)
{
    m_factoryDesc = dto.getRtti();
    if (const auto v = dto.tryGetValue<unsigned>(TOKEN_HAND_SYSTEM)) m_handSys = static_cast<GraphicCoordSys>(v.value());
    if (const auto v = dto.tryGetValue<unsigned>(TOKEN_PROJECTION_TYPE)) m_projectionType = static_cast<Frustum::ProjectionType>(v.value());
    if (const auto v = dto.tryGetValue<float>(TOKEN_FOV)) m_fov = MathLib::Radian(v.value());
    if (const auto v = dto.tryGetValue<float>(TOKEN_NEAR_PLANE_Z)) m_nearPlaneZ = v.value();
    if (const auto v = dto.tryGetValue<float>(TOKEN_FAR_PLANE_Z)) m_farPlaneZ = v.value();
    if (const auto v = dto.tryGetValue<float>(TOKEN_ASPECT_RATIO)) m_aspectRatio = v.value();
    if (const auto v = dto.tryGetValue<float>(TOKEN_NEAR_WIDTH)) m_nearWidth = v.value();
    if (const auto v = dto.tryGetValue<float>(TOKEN_NEAR_HEIGHT)) m_nearHeight = v.value();
}
