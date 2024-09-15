#include "AvatarRecipeAssemblers.h"
#include "AvatarRecipes.h"

using namespace Enigma::GameCommon;

static std::string TOKEN_OLD_MATERIAL_ID = "OldMaterialId";
static std::string TOKEN_NEW_MATERIAL_ID = "NewMaterialId";
static std::string TOKEN_MESH_ID = "MeshId";
static std::string TOKEN_TEXTURE_MAPPING_DTO = "TextureMappingDto";

AvatarRecipeAssembler::AvatarRecipeAssembler() : m_factoryDesc(AvatarRecipe::TYPE_RTTI)
{
}

Enigma::Engine::GenericDto AvatarRecipeAssembler::assemble() const
{
    Enigma::Engine::GenericDto dto;
    dto.addRtti(m_factoryDesc);
    return dto;
}

ReplaceAvatarMaterialAssembler::ReplaceAvatarMaterialAssembler() : AvatarRecipeAssembler()
{
    m_factoryDesc = Engine::FactoryDesc(ReplaceAvatarMaterial::TYPE_RTTI);
}

Enigma::Engine::GenericDto ReplaceAvatarMaterialAssembler::assemble() const
{
    Enigma::Engine::GenericDto dto = AvatarRecipeAssembler::assemble();
    dto.addOrUpdate(TOKEN_OLD_MATERIAL_ID, m_oldMaterialId.name());
    dto.addOrUpdate(TOKEN_NEW_MATERIAL_ID, m_newMaterialId.name());
    return dto;
}

