/*********************************************************************
 * \file   AvatarRecipies.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   April 2023
 *********************************************************************/
#ifndef _AVATAR_RECIPES_H
#define _AVATAR_RECIPES_H

#include "SceneGraph/Pawn.h"
#include "GameEngine/EffectMaterialDto.h"

namespace Enigma::GameCommon
{
    class AvatarRecipe
    {
    public:
        AvatarRecipe() = default;
        virtual ~AvatarRecipe() = default;

        virtual void Bake(const std::shared_ptr<Enigma::SceneGraph::Pawn>& pawn) = 0;
    };

    class ReplaceAvatarMaterial : public AvatarRecipe
    {
    public:
        ReplaceAvatarMaterial(const std::string& old_material_name, const Engine::EffectMaterialDto& new_material_dto)
            : m_oldMaterialName(old_material_name), m_newMaterialDto(new_material_dto) {}

        void Bake(const std::shared_ptr<Enigma::SceneGraph::Pawn>& pawn) override;

    protected:
        std::string m_oldMaterialName;
        Engine::EffectMaterialDto m_newMaterialDto;
    };
}

#endif // _AVATAR_RECIPES_H
