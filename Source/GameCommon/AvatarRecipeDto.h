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

namespace Enigma::GameCommon
{
    class AvatarRecipeDto
    {
    public:
        AvatarRecipeDto() = default;
        AvatarRecipeDto(const AvatarRecipeDto&) = default;
        AvatarRecipeDto(AvatarRecipeDto&&) = default;
        virtual ~AvatarRecipeDto() = default;
        AvatarRecipeDto& operator=(const AvatarRecipeDto&) = default;
        AvatarRecipeDto& operator=(AvatarRecipeDto&&) = default;
    };

    class AvatarRecipeReplaceMaterialDto : public AvatarRecipeDto
    {
    public:
        AvatarRecipeReplaceMaterialDto() = default;

        std::string& OldMaterialName() { return m_oldMaterialName; }
        [[nodiscard]] const std::string& OldMaterialName() const { return m_oldMaterialName; }
        Engine::EffectMaterialDto& NewMaterialDto() { return m_newMaterialDto; }
        [[nodiscard]] const Engine::EffectMaterialDto& NewMaterialDto() const { return m_newMaterialDto; }

        static AvatarRecipeReplaceMaterialDto FromGenericDto(const Engine::GenericDto& dto);
        Engine::GenericDto ToGenericDto() const;

    private:
        std::string m_oldMaterialName;
        Engine::EffectMaterialDto m_newMaterialDto;
    };

    class AvatarRecipeChangeTextureDto : public AvatarRecipeDto
    {
    public:
        AvatarRecipeChangeTextureDto() = default;

        std::string& MeshName() { return m_meshName; }
        [[nodiscard]] const std::string& MeshName() const { return m_meshName; }
        Engine::TextureMappingDto& TextureDto() { return m_textureDto; }
        [[nodiscard]] const Engine::TextureMappingDto& TextureDto() const { return m_textureDto; }

        static AvatarRecipeChangeTextureDto FromGenericDto(const Engine::GenericDto& dto);
        Engine::GenericDto ToGenericDto() const;
    private:
        std::string m_meshName;
        Engine::TextureMappingDto m_textureDto;
    };
}

#endif // _AVATAR_RECIPE_DTO_H
