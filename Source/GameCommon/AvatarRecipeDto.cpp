#include "AvatarRecipeDto.h"
#include "AvatarRecipes.h"

using namespace Enigma::GameCommon;
using namespace Enigma::Engine;

static std::string TOKEN_OLD_MATERIAL_ID = "OldMaterialId";
static std::string TOKEN_NEW_MATERIAL_ID = "NewMaterialId";
static std::string TOKEN_MESH_NAME = "MeshName";
static std::string TOKEN_TEXTURE_MAPPING_DTO = "TextureMappingDto";

AvatarRecipeDto::AvatarRecipeDto() : m_factoryDesc(AvatarRecipe::TYPE_RTTI.getName())
{
}

AvatarRecipeReplaceMaterialDto::AvatarRecipeReplaceMaterialDto() : AvatarRecipeDto()
{
    m_factoryDesc = Engine::FactoryDesc(ReplaceAvatarMaterial::TYPE_RTTI.getName());
}

AvatarRecipeReplaceMaterialDto AvatarRecipeReplaceMaterialDto::fromGenericDto(const Engine::GenericDto& dto)
{
    AvatarRecipeReplaceMaterialDto recipe;
    if (auto v = dto.TryGetValue<std::string>(TOKEN_OLD_MATERIAL_ID)) recipe.oldMaterialId() = v.value();
    if (auto v = dto.TryGetValue<std::string>(TOKEN_NEW_MATERIAL_ID)) recipe.newMaterialId() = v.value();
    return recipe;
}

GenericDto AvatarRecipeReplaceMaterialDto::toGenericDto() const
{
    GenericDto dto;
    dto.AddRtti(m_factoryDesc);
    dto.AddOrUpdate(TOKEN_OLD_MATERIAL_ID, m_oldMaterialId.name());
    dto.AddOrUpdate(TOKEN_NEW_MATERIAL_ID, m_newMaterialId.name());
    return dto;
}

AvatarRecipeChangeTextureDto::AvatarRecipeChangeTextureDto() : AvatarRecipeDto()
{
    m_factoryDesc = Engine::FactoryDesc(ChangeAvatarTexture::TYPE_RTTI.getName());
}

AvatarRecipeChangeTextureDto AvatarRecipeChangeTextureDto::fromGenericDto(const Engine::GenericDto& dto)
{
    AvatarRecipeChangeTextureDto recipe;
    if (auto v = dto.TryGetValue<std::string>(TOKEN_MESH_NAME)) recipe.MeshName() = v.value();
    if (auto v = dto.TryGetValue<GenericDto>(TOKEN_TEXTURE_MAPPING_DTO)) recipe.TextureDto() = TextureMappingDto::fromGenericDto(v.value());
    return recipe;
}

GenericDto AvatarRecipeChangeTextureDto::toGenericDto() const
{
    GenericDto dto;
    dto.AddRtti(m_factoryDesc);
    dto.AddOrUpdate(TOKEN_MESH_NAME, m_meshName);
    dto.AddOrUpdate(TOKEN_TEXTURE_MAPPING_DTO, m_textureDto.toGenericDto());
    return dto;
}
