/*****************************************************************
 * \file   CameraAssembler.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   June 2024
 ******************************************************************/
#ifndef CAMERA_ASSEMBLER_H
#define CAMERA_ASSEMBLER_H

#include "GameEngine/GenericDto.h"
#include "SpatialId.h"
#include "MathLib/Vector3.h"
#include "SceneGraphDefines.h"

namespace Enigma::SceneGraph
{
    class Frustum;
    class FrustumAssembler;

    class CameraAssembler
    {
    public:
        CameraAssembler(const SpatialId& id);

        void factoryDesc(const Engine::FactoryDesc& factory_desc) { m_factoryDesc = factory_desc; }
        void handSystem(GraphicCoordSys hand_system) { m_handSys = hand_system; }
        void eyePosition(const MathLib::Vector3& eye_position) { m_eyePosition = eye_position; }
        void lookAt(const MathLib::Vector3& look_at) { m_lookAtDir = look_at; }
        void upDirection(const MathLib::Vector3& up_direction) { m_up = up_direction; }
        void frustum(const std::shared_ptr<FrustumAssembler>& frustum);
        void frustum(const Frustum& frustum);
        void asNative(const std::string& file_at_path);

        Engine::GenericDto assemble();

    protected:
        Engine::FactoryDesc m_factoryDesc;
        SpatialId m_id;
        GraphicCoordSys m_handSys;
        MathLib::Vector3 m_eyePosition;
        MathLib::Vector3 m_lookAtDir;
        MathLib::Vector3 m_up;
        std::shared_ptr<FrustumAssembler> m_frustumAssembler;
    };
    class CameraDisassembler
    {
    public:
        CameraDisassembler();
        CameraDisassembler(const Engine::GenericDto& dto);

        SpatialId id() const { return m_id; }
        const Engine::FactoryDesc& factoryDesc() const { return m_factoryDesc; }
        GraphicCoordSys handSystem() const { return m_handSys; }
        const MathLib::Vector3& eyePosition() const { return m_eyePosition; }
        const MathLib::Vector3& lookAt() const { return m_lookAtDir; }
        const MathLib::Vector3& upDirection() const { return m_up; }
        const Frustum& frustum() const;

        void disassemble(const Engine::GenericDto& dto);

    protected:
        Engine::FactoryDesc m_factoryDesc;
        SpatialId m_id;
        GraphicCoordSys m_handSys;
        MathLib::Vector3 m_eyePosition;
        MathLib::Vector3 m_lookAtDir;
        MathLib::Vector3 m_up;
        std::shared_ptr<Frustum> m_frustum;
    };
}

#endif // CAMERA_ASSEMBLER_H
