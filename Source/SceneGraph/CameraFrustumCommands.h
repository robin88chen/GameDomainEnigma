/*********************************************************************
 * \file   CameraFrustumCommands.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   September 2023
 *********************************************************************/
#ifndef CAMERA_FRUSTUM_COMMANDS_H
#define CAMERA_FRUSTUM_COMMANDS_H

#include "Frameworks/Command.h"
#include "GameEngine/GenericDto.h"
#include "SpatialId.h"

namespace Enigma::SceneGraph
{
    class CreateCamera : public Frameworks::ICommand
    {
    public:
        CreateCamera(const SpatialId& id) : m_id(id) {}

        const SpatialId& id() { return m_id; }

    protected:
        SpatialId m_id;
    };
    class ConstituteCamera : public Frameworks::ICommand
    {
    public:
        ConstituteCamera(const SpatialId& id, const Engine::GenericDto& dto) : m_id(id), m_dto(dto) {}

        const SpatialId& id() { return m_id; }
        const Engine::GenericDto& dto() { return m_dto; }

    protected:
        SpatialId m_id;
        Engine::GenericDto m_dto;
    };
}

#endif // CAMERA_FRUSTUM_COMMANDS_H
