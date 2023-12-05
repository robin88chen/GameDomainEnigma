/*********************************************************************
 * \file   RenderableCommands.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2023
 *********************************************************************/
#ifndef RENDERABLE_COMMANDS_H
#define RENDERABLE_COMMANDS_H

#include "Frameworks/Command.h"
#include "GameEngine/GenericDto.h"

namespace Enigma::Renderer
{
    class BuildRenderablePrimitive : public Frameworks::IRequestCommand
    {
    public:
        BuildRenderablePrimitive(const Engine::GenericDto& primitive_dto) : m_primitiveDto(primitive_dto) {}
        const Engine::GenericDto& primitiveDto() { return m_primitiveDto; }

    private:
        Engine::GenericDto m_primitiveDto;
    };
}

#endif // RENDERABLE_COMMANDS_H
