/*********************************************************************
 * \file   WorldMapCommands.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   July 2023
 *********************************************************************/
#ifndef WORLD_MAP_COMMANDS_H
#define WORLD_MAP_COMMANDS_H

#include "Frameworks/Command.h"
#include "GameEngine/GenericDto.h"

namespace Enigma::WorldMap
{
    class CreateEmptyWorldMap : public Frameworks::ICommand
    {
    public:
        CreateEmptyWorldMap(const Engine::GenericDto& dto) : m_dto(dto) {}

        const Engine::GenericDto& GetDto() const { return m_dto; }

    protected:
        Engine::GenericDto m_dto;
    };
}

#endif // WORLD_MAP_COMMANDS_H
