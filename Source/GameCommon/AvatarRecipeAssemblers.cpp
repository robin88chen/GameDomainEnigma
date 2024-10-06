#include "AvatarRecipeAssemblers.h"
#include "AvatarRecipes.h"
#include "GameEngine/EffectSemanticTextureAssembler.h"

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

ChangeAvatarTextureAssembler::ChangeAvatarTextureAssembler() : AvatarRecipeAssembler()
{
    m_factoryDesc = Engine::FactoryDesc(ChangeAvatarTexture::TYPE_RTTI);
}

Enigma::Engine::GenericDto ChangeAvatarTextureAssembler::assemble() const
{
    Enigma::Engine::GenericDto dto = AvatarRecipeAssembler::assemble();
    dto.addOrUpdate(TOKEN_MESH_ID, m_meshId.tokens());
    std::shared_ptr<Engine::EffectSemanticTextureAssembler> textureAssembler = std::make_shared<Engine::EffectSemanticTextureAssembler>(m_semanticTexture);
    dto.addOrUpdate(TOKEN_TEXTURE_MAPPING_DTO, textureAssembler->assemble());
    return dto;
}

AvatarRecipeDisassembler::AvatarRecipeDisassembler() : m_factoryDesc(AvatarRecipe::TYPE_RTTI)
{
}

void AvatarRecipeDisassembler::disassemble(const Enigma::Engine::GenericDto& dto)
{
    m_factoryDesc = dto.getRtti();
}

ReplaceAvatarMaterialDisassembler::ReplaceAvatarMaterialDisassembler() : AvatarRecipeDisassembler()
{
    m_factoryDesc = Engine::FactoryDesc(ReplaceAvatarMaterial::TYPE_RTTI);
}

void ReplaceAvatarMaterialDisassembler::disassemble(const Engine::GenericDto& dto)
{
    AvatarRecipeDisassembler::disassemble(dto);
    if (const auto v = dto.tryGetValue<std::string>(TOKEN_OLD_MATERIAL_ID)) m_oldMaterialId = v.value();
    if (const auto v = dto.tryGetValue<std::string>(TOKEN_NEW_MATERIAL_ID)) m_newMaterialId = v.value();
}

ChangeAvatarTextureDisassembler::ChangeAvatarTextureDisassembler() : AvatarRecipeDisassembler()
{
    m_factoryDesc = Engine::FactoryDesc(ChangeAvatarTexture::TYPE_RTTI);
}

void ChangeAvatarTextureDisassembler::disassemble(const Engine::GenericDto& dto)
{
    AvatarRecipeDisassembler::disassemble(dto);
    if (const auto v = dto.tryGetValue<std::vector<std::string>>(TOKEN_MESH_ID)) m_meshId = v.value();
    if (const auto v = dto.tryGetValue<Engine::GenericDto>(TOKEN_TEXTURE_MAPPING_DTO))
    {
        Engine::EffectSemanticTextureDisassembler textureDisassembler(v.value());
        m_semanticTexture = textureDisassembler.semanticTexture();
    }
}
