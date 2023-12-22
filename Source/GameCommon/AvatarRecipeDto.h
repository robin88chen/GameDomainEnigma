/*********************************************************************
 * \file   AvatarRecipeDto.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   April 2023
 *********************************************************************/
#ifndef _AVATAR_RECIPE_DTO_H
#define _AVATAR_RECIPE_DTO_H

#include "GameEngine/EffectMaterialDto.h"
#include "GameEngine/EffectTextureMapDto.h"
#include "GameEngine/EffectMaterialId.h"

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

        std::string& MeshName() { return m_meshName; }
        [[nodiscard]] const std::string& MeshName() const { return m_meshName; }
        Engine::TextureMappingDto& TextureDto() { return m_textureDto; }
        [[nodiscard]] const Engine::TextureMappingDto& TextureDto() const { return m_textureDto; }

        static AvatarRecipeChangeTextureDto fromGenericDto(const Engine::GenericDto& dto);
        Engine::GenericDto toGenericDto() const;
    private:
        std::string m_meshName;
        Engine::TextureMappingDto m_textureDto;
    };
}

#endif // _AVATAR_RECIPE_DTO_H
