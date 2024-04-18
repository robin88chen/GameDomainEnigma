/*********************************************************************
 * \file   CameraFrustumDtos.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   December 2022
 *********************************************************************/
#ifndef _CAMERA_FRUSTUM_DTOS_H
#define _CAMERA_FRUSTUM_DTOS_H

#include "SceneGraphRepository.h"
#include "GameEngine/GenericDto.h"
#include "Frustum.h"

namespace Enigma::SceneGraph
{
    class CameraDto
    {
    public:
        CameraDto();
        CameraDto(const CameraDto&) = default;
        CameraDto(CameraDto&&) = default;
        ~CameraDto() = default;
        CameraDto& operator=(const CameraDto&) = default;
        CameraDto& operator=(CameraDto&&) = default;

        [[nodiscard]] const Engine::FactoryDesc& factoryDesc() const { return m_factoryDesc; }
        Engine::FactoryDesc& factoryDesc() { return m_factoryDesc; }

        SpatialId& id() { return m_id; }
        [[nodiscard]] const SpatialId& id() const { return m_id; }
        Enigma::SceneGraph::GraphicCoordSys& HandSystem() { return m_handSys; }
        [[nodiscard]] Enigma::SceneGraph::GraphicCoordSys HandSystem() const { return m_handSys; }
        MathLib::Vector3& EyePosition() { return m_eyePosition; }
        [[nodiscard]] MathLib::Vector3 EyePosition() const { return m_eyePosition; }
        MathLib::Vector3& LookAtDirection() { return m_lookAtDir; }
        [[nodiscard]] MathLib::Vector3 LookAtDirection() const { return m_lookAtDir; }
        MathLib::Vector3& UpVector() { return m_up; }
        [[nodiscard]] MathLib::Vector3 UpVector() const { return m_up; }
        Engine::GenericDto& Frustum() { return m_frustumDto; }
        [[nodiscard]] Engine::GenericDto Frustum() const { return m_frustumDto; }

        static CameraDto fromGenericDto(const Engine::GenericDto& dto);
        Engine::GenericDto toGenericDto();

    protected:
        Engine::FactoryDesc m_factoryDesc;
        SpatialId m_id;
        Enigma::SceneGraph::GraphicCoordSys m_handSys;
        MathLib::Vector3 m_eyePosition;
        MathLib::Vector3 m_lookAtDir;
        MathLib::Vector3 m_up;
        Engine::GenericDto m_frustumDto;
    };

    class FrustumDto
    {
    public:
        FrustumDto();
        FrustumDto(const FrustumDto&) = default;
        FrustumDto(FrustumDto&&) = default;
        ~FrustumDto() = default;
        FrustumDto& operator=(const FrustumDto&) = default;
        FrustumDto& operator=(FrustumDto&&) = default;

        [[nodiscard]] const Engine::FactoryDesc& factoryDesc() const { return m_factoryDesc; }
        Engine::FactoryDesc& factoryDesc() { return m_factoryDesc; }

        GraphicCoordSys& HandSystem() { return m_handSys; }
        [[nodiscard]] GraphicCoordSys HandSystem() const { return m_handSys; }
        Frustum::ProjectionType& ProjectionType() { return m_projectionType; }
        [[nodiscard]] Frustum::ProjectionType ProjectionType() const { return m_projectionType; }

        float& Fov() { return m_fov; }
        [[nodiscard]] float Fov() const { return m_fov; }
        float& NearPlaneZ() { return m_nearPlaneZ; }
        [[nodiscard]] float NearPlaneZ() const { return m_nearPlaneZ; }
        float& FarPlaneZ() { return m_farPlaneZ; }
        [[nodiscard]] float FarPlaneZ() const { return m_farPlaneZ; }
        float& AspectRatio() { return m_aspectRatio; }
        [[nodiscard]] float AspectRatio() const { return m_aspectRatio; }
        float& NearWidth() { return m_nearWidth; }
        [[nodiscard]] float NearWidth() const { return m_nearWidth; }
        float& NearHeight() { return m_nearHeight; }
        [[nodiscard]] float NearHeight() const { return m_nearHeight; }

        static FrustumDto fromGenericDto(const Engine::GenericDto& dto);
        Engine::GenericDto toGenericDto();

    protected:
        Engine::FactoryDesc m_factoryDesc;
        GraphicCoordSys m_handSys;
        Frustum::ProjectionType m_projectionType;
        float m_fov;
        float m_nearPlaneZ;
        float m_farPlaneZ;
        float m_aspectRatio;
        float m_nearWidth;
        float m_nearHeight;
    };
}

#endif // _CAMERA_FRUSTUM_DTOS_H
