#include "AvatarRecipeDto.h"
#include "AvatarRecipes.h"

using namespace Enigma::GameCommon;
using namespace Enigma::Engine;

static std::string TOKEN_OLD_MATERIAL_ID = "OldMaterialId";
static std::string TOKEN_NEW_MATERIAL_ID = "NewMaterialId";
static std::string TOKEN_MESH_ID = "MeshId";
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
    if (auto v = dto.tryGetValue<std::string>(TOKEN_OLD_MATERIAL_ID)) recipe.oldMaterialId() = v.value();
    if (auto v = dto.tryGetValue<std::string>(TOKEN_NEW_MATERIAL_ID)) recipe.newMaterialId() = v.value();
    return recipe;
}

GenericDto AvatarRecipeReplaceMaterialDto::toGenericDto() const
{
    GenericDto dto;
    dto.addRtti(m_factoryDesc);
    dto.addOrUpdate(TOKEN_OLD_MATERIAL_ID, m_oldMaterialId.name());
    dto.addOrUpdate(TOKEN_NEW_MATERIAL_ID, m_newMaterialId.name());
    return dto;
}

AvatarRecipeChangeTextureDto::AvatarRecipeChangeTextureDto() : AvatarRecipeDto()
{
    m_factoryDesc = Engine::FactoryDesc(ChangeAvatarTexture::TYPE_RTTI.getName());
}

AvatarRecipeChangeTextureDto AvatarRecipeChangeTextureDto::fromGenericDto(const Engine::GenericDto& dto)
{
    AvatarRecipeChangeTextureDto recipe;
    if (auto v = dto.tryGetValue<std::vector<std::string>>(TOKEN_MESH_ID)) recipe.meshId() = v.value();
    if (auto v = dto.tryGetValue<GenericDto>(TOKEN_TEXTURE_MAPPING_DTO)) recipe.textureDto() = TextureMappingDto::fromGenericDto(v.value());
    return recipe;
}

GenericDto AvatarRecipeChangeTextureDto::toGenericDto() const
{
    GenericDto dto;
    dto.addRtti(m_factoryDesc);
    dto.addOrUpdate(TOKEN_MESH_ID, m_meshId.tokens());
    dto.addOrUpdate(TOKEN_TEXTURE_MAPPING_DTO, m_textureDto.toGenericDto());
    return dto;
}
