#include "PrimitiveMaterialAssembler.h"
#include "PrimitiveMaterial.h"
#include <cassert>

using namespace Enigma::Renderables;

static std::string TOKEN_EFFECT = "Effect";
static std::string TOKEN_TEXTURE_MAP = "TextureMap";

PrimitiveMaterialAssembler::PrimitiveMaterialAssembler(const std::shared_ptr<Engine::EffectMaterial>& effectMaterial, const Engine::EffectTextureMap& effectTextureMap)
{
    m_effectMaterialId = effectMaterial->id();
    if (!effectTextureMap.isAllResourceTexture())
    {
        m_effectTextureMap = std::make_shared<Engine::EffectTextureMapAssembler>();
        effectTextureMap.assemble(m_effectTextureMap);
    }
}

void PrimitiveMaterialAssembler::effectTextureMap(const Engine::EffectTextureMap& textureMap)
{
    if (!textureMap.isAllResourceTexture())
    {
        m_effectTextureMap = std::make_shared<Engine::EffectTextureMapAssembler>();
        textureMap.assemble(m_effectTextureMap);
    }
}

Enigma::Engine::GenericDto PrimitiveMaterialAssembler::assemble() const
{
    Enigma::Engine::GenericDto dto;
    dto.addOrUpdate(TOKEN_EFFECT, m_effectMaterialId.name());
    if (m_effectTextureMap) dto.addOrUpdate(TOKEN_TEXTURE_MAP, m_effectTextureMap->assemble());
    return dto;
}

std::shared_ptr<PrimitiveMaterialAssembler> PrimitiveMaterialAssembler::assembledAssemblerOf(const std::shared_ptr<PrimitiveMaterial>& material)
{
    assert(material);
    auto assembler = material->assembler();
    material->assemble(assembler);
    return assembler;
}

PrimitiveMaterialDisassembler::PrimitiveMaterialDisassembler(const Enigma::Engine::GenericDto& dto)
{
    disassemble(dto);
}

void PrimitiveMaterialDisassembler::disassemble(const Enigma::Engine::GenericDto& dto)
{
    if (const auto v = dto.tryGetValue<Engine::EffectMaterialId>(TOKEN_EFFECT)) m_effectMaterialId = v.value();
    if (const auto v = dto.tryGetValue<Enigma::Engine::GenericDto>(TOKEN_TEXTURE_MAP))
    {
        std::shared_ptr<Engine::EffectTextureMapDisassembler> disassembler = std::make_shared<Engine::EffectTextureMapDisassembler>();
        disassembler->disassemble(v.value());
        m_effectTextureMap.disassemble(disassembler);
    }
}