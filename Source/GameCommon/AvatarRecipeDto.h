/*********************************************************************
 * \file   AvatarRecipeDto.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   April 2023
 *********************************************************************/
#ifndef _AVATAR_RECIPE_DTO_H
#define _AVATAR_RECIPE_DTO_H

#include "GameEngine/EffectTextureMapAssembler.h"
#include "GameEngine/EffectMaterialId.h"
#include "Primitives/Primitive.h"

namespace Enigma::GameCommon
{
    class AvatarRecipeDto
    {
    public:
        AvatarRecipeDto();
        AvatarRecipeDto(const AvatarRecipeDto&) = default;
        AvatarRecipeDto(AvatarRecipeDto&&) = default;
        virtual ~AvatarRecipeDto() = default;
        AvatarRecipeDto& operator=(const AvatarRecipeDto&) = default;
        AvatarRecipeDto& operator=(AvatarRecipeDto&&) = default;

        [[nodiscard]] const Engine::FactoryDesc& factoryDesc() const { return m_factoryDesc; }
        Engine::FactoryDesc& factoryDesc() { return m_factoryDesc; }

    protected:
        Engine::FactoryDesc m_factoryDesc;
    };

    class AvatarRecipeReplaceMaterialDto : public AvatarRecipeDto
    {
    public:
        AvatarRecipeReplaceMaterialDto();

        Engine::EffectMaterialId& oldMaterialId() { return m_oldMaterialId; }
        [[nodiscard]] const Engine::EffectMaterialId& oldMaterialId() const { return m_oldMaterialId; }
        Engine::EffectMaterialId& newMaterialId() { return m_newMaterialId; }
        [[nodiscard]] const Engine::EffectMaterialId& newMaterialId() const { return m_newMaterialId; }

        static AvatarRecipeReplaceMaterialDto fromGenericDto(const Engine::GenericDto& dto);
        Engine::GenericDto toGenericDto() const;

    private:
        Engine::EffectMaterialId m_oldMaterialId;
        Engine::EffectMaterialId m_newMaterialId;
    };

    class AvatarRecipeChangeTextureDto : public AvatarRecipeDto
    {
    public:
        AvatarRecipeChangeTextureDto();

        Primitives::PrimitiveId& meshId() { return m_meshId; }
        [[nodiscard]] const Primitives::PrimitiveId& meshId() const { return m_meshId; }
        Engine::TextureMappingDisassembler& textureDto() { return m_textureDto; }
        [[nodiscard]] const Engine::TextureMappingDisassembler& textureDto() const { return m_textureDto; }

        static AvatarRecipeChangeTextureDto fromGenericDto(const Engine::GenericDto& dto);
        Engine::GenericDto toGenericDto() const;
    private:
        Primitives::PrimitiveId m_meshId;
        Engine::TextureMappingDisassembler m_textureDto;
    };
}

#endif // _AVATAR_RECIPE_DTO_H
