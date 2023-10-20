/*********************************************************************
 * \file   PrefabCommands.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   October 2023
 *********************************************************************/
#ifndef PREFAB_COMMANDS_H
#define PREFAB_COMMANDS_H

#include "Frameworks\Command.h"
#include "GameEngine\GenericDto.h"

namespace Enigma::Prefabs
{
    class LoadPawnPrefab : public Frameworks::IRequestCommand
    {
    public:
        LoadPawnPrefab(const Engine::GenericDto& pawn_dto) : m_pawnDto(pawn_dto) {}

        const Engine::GenericDto& GetPawnDto() const { return m_pawnDto; }
    protected:
        Engine::GenericDto m_pawnDto;
    };
}

#endif // PREFAB_COMMANDS_H
