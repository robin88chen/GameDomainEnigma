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

namespace Enigma::SceneGraph
{
    class CreateCamera : public Frameworks::ICommand
    {
    public:
        CreateCamera(const Engine::GenericDto& dto) : m_dto(dto) {}

        const Engine::GenericDto& GetDto() { return m_dto; }

    protected:
        Engine::GenericDto m_dto;
    };
}

#endif // CAMERA_FRUSTUM_COMMANDS_H
