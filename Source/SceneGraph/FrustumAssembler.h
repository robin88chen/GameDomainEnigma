/*********************************************************************
 * \file   FrustumAssembler.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   August 2024
 *********************************************************************/
#ifndef FRUSTUM_ASSEMBLER_H
#define FRUSTUM_ASSEMBLER_H

#include "Frustum.h"

namespace Enigma::SceneGraph
{
    class FrustumAssembler
    {
    public:
        FrustumAssembler(Frustum::ProjectionType projection_type);

        void factoryDesc(const Engine::FactoryDesc& factory_desc) { m_factoryDesc = factory_desc; }
        void projectionType(Frustum::ProjectionType projection_type) { m_projectionType = projection_type; }
        void handSystem(GraphicCoordSys hand_system) { m_handSys = hand_system; }
        void frontBackZ(float front_z, float back_z);
        void fov(MathLib::Radian fov);
        void nearPlaneDimension(float width, float height);

        Engine::GenericDto assemble();

    protected:
        Engine::FactoryDesc m_factoryDesc;
        GraphicCoordSys m_handSys;
        Frustum::ProjectionType m_projectionType;
        MathLib::Radian m_fov;
        float m_nearPlaneZ;
        float m_farPlaneZ;
        float m_aspectRatio;
        float m_nearWidth;
        float m_nearHeight;
    };
    class FrustumDisassembler
    {
    public:
        FrustumDisassembler();
        FrustumDisassembler(const Engine::GenericDto& dto);

        const Engine::FactoryDesc& factoryDesc() const { return m_factoryDesc; }
        Frustum::ProjectionType projectionType() const { return m_projectionType; }
        GraphicCoordSys handSystem() const { return m_handSys; }
        float nearPlaneZ() const { return m_nearPlaneZ; }
        float farPlaneZ() const { return m_farPlaneZ; }
        MathLib::Radian fov() const { return m_fov; }
        float aspectRatio() const { return m_aspectRatio; }
        float nearWidth() const { return m_nearWidth; }
        float nearHeight() const { return m_nearHeight; }

        void disassemble(const Engine::GenericDto& dto);

    protected:
        Engine::FactoryDesc m_factoryDesc;
        GraphicCoordSys m_handSys;
        Frustum::ProjectionType m_projectionType;
        MathLib::Radian m_fov;
        float m_nearPlaneZ;
        float m_farPlaneZ;
        float m_aspectRatio;
        float m_nearWidth;
        float m_nearHeight;
    };
}

#endif // FRUSTUM_ASSEMBLER_H
