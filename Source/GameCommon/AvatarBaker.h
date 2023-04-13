/*********************************************************************
 * \file   AvatarBaker.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   April 2023
 *********************************************************************/
#ifndef _AVATAR_BAKER_H
#define _AVATAR_BAKER_H

#include "SceneGraph/Pawn.h"
#include "GameEngine/EffectMaterialDto.h"

namespace Enigma::GameCommon
{
    class AvatarBaker
    {
    public:
        static void ReplaceAvatarMaterial(const std::shared_ptr<SceneGraph::Pawn>& pawn,
            const std::string& old_material_name, const Engine::EffectMaterialDto& new_material_dto);
    };
}

#endif // _AVATAR_BAKER_H
