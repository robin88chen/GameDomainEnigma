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
#include <string>

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

    class QueryCamera : public Frameworks::IRequestCommand
    {
    public:
        QueryCamera(const std::string& name) : m_name(name) {}

        const std::string& GetName() { return m_name; }

    protected:
        std::string m_name;
    };
}

#endif // CAMERA_FRUSTUM_COMMANDS_H
