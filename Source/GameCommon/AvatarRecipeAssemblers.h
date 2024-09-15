/*********************************************************************
 * \file   AvatarRecipeAssemblers.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   September 2024
 *********************************************************************/
#ifndef AVATAR_RECIPE_ASSEMBLERS_H
#define AVATAR_RECIPE_ASSEMBLERS_H

#include "GameEngine/GenericDto.h"
#include "GameEngine/EffectMaterialId.h"

namespace Enigma::GameCommon
{
    class AvatarRecipeAssembler
    {
    public:
        AvatarRecipeAssembler();
        virtual ~AvatarRecipeAssembler() = default;

        void factory(const Engine::FactoryDesc& factoryDesc) { m_factoryDesc = factoryDesc; }

        virtual Engine::GenericDto assemble() const;

    protected:
        Engine::FactoryDesc m_factoryDesc;
    };
    class ReplaceAvatarMaterialAssembler : public AvatarRecipeAssembler
    {
    public:
        ReplaceAvatarMaterialAssembler();
        virtual ~ReplaceAvatarMaterialAssembler() = default;

        void oldMaterialId(const Engine::EffectMaterialId& oldMaterialId) { m_oldMaterialId = oldMaterialId; }
        void newMaterialId(const Engine::EffectMaterialId& newMaterialId) { m_newMaterialId = newMaterialId; }

        Engine::GenericDto assemble() const override;

    protected:
        Engine::EffectMaterialId m_oldMaterialId;
        Engine::EffectMaterialId m_newMaterialId;
    };
}

#endif // AVATAR_RECIPE_ASSEMBLERS_H
