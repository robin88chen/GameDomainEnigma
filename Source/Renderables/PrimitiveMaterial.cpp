#include "PrimitiveMaterial.h"
#include "PrimitiveMaterialAssembler.h"
#include "RenderableErrors.h"
#include "GameEngine/Texture.h"

using namespace Enigma::Renderables;

DEFINE_RTTI_OF_BASE(Renderables, PrimitiveMaterial);

PrimitiveMaterial::PrimitiveMaterial() : m_effectMaterial(nullptr), m_effectTextureMap()
{
}

PrimitiveMaterial::PrimitiveMaterial(const std::shared_ptr<Engine::EffectMaterial>& effectMaterial, const Engine::EffectTextureMap& effectTextureMap) : m_effectMaterial(effectMaterial), m_effectTextureMap(effectTextureMap)
{
}

PrimitiveMaterial::PrimitiveMaterial(const Engine::EffectMaterialId& effectMaterialId, const Engine::EffectTextureMap& effectTextureMap)
{
    m_effectMaterial = Engine::EffectMaterial::queryEffectMaterial(effectMaterialId);
    m_effectTextureMap = effectTextureMap;
}

std::shared_ptr<PrimitiveMaterialAssembler> PrimitiveMaterial::assembler() const
{
    return std::make_shared<PrimitiveMaterialAssembler>(m_effectMaterial, m_effectTextureMap);
}

void PrimitiveMaterial::assemble(const std::shared_ptr<PrimitiveMaterialAssembler>& assembler) const
{
    assert(assembler);
    assembler->effectMaterialId(m_effectMaterial->id());
    assembler->effectTextureMap(m_effectTextureMap);
}

std::shared_ptr<PrimitiveMaterialDisassembler> PrimitiveMaterial::disassembler() const
{
    return std::make_shared<PrimitiveMaterialDisassembler>();
}

void PrimitiveMaterial::disassemble(const std::shared_ptr<PrimitiveMaterialDisassembler>& disassembler)
{
    assert(disassembler);
    m_effectMaterial = Engine::EffectMaterial::queryEffectMaterial(disassembler->effectMaterialId());
    m_effectTextureMap = disassembler->effectTextureMap();
}

std::error_code PrimitiveMaterial::assignShaderTextures()
{
    if (m_effectMaterial == nullptr) return ErrorCode::nullEffectMaterial;
    for (unsigned i = 0; i < m_effectTextureMap.getCount(); i++)
    {
        auto& eff_tex_set = m_effectTextureMap.getEffectSemanticTextureTuple(i);
        if (std::get<std::shared_ptr<Engine::Texture>>(eff_tex_set) == nullptr) continue;
        // 改直接指定
        m_effectMaterial->assignVariableValue(std::get<std::string>(eff_tex_set), Graphics::IShaderVariable::TextureVarTuple{
                    std::get<std::shared_ptr<Engine::Texture>>(eff_tex_set)->getDeviceTexture(), std::get<std::optional<unsigned>>(eff_tex_set) });
        /*(*eff_iter)->setVariableAssignFunc(std::get<std::string>(eff_tex_set),
            [=](auto& var)
            {
                var.AssignValue(IShaderVariable::TextureVarTuple{
                    std::get<TexturePtr>(eff_tex_set)->GetDeviceTexture(), std::get<std::optional<unsigned>>(eff_tex_set) });
            });*/
    }
    return ErrorCode::ok;
}

std::error_code PrimitiveMaterial::unassignShaderTextures()
{
    if (m_effectMaterial == nullptr) return ErrorCode::nullEffectMaterial;
    for (unsigned i = 0; i < m_effectTextureMap.getCount(); i++)
    {
        auto& eff_tex_set = m_effectTextureMap.getEffectSemanticTextureTuple(i);
        // 改直接指定
        m_effectMaterial->assignVariableValue(std::get<std::string>(eff_tex_set), Graphics::IShaderVariable::TextureVarTuple{ nullptr, std::nullopt });
        /*(*eff_iter)->setVariableAssignFunc(std::get<std::string>(eff_tex_set),
            [=](auto& var)
            {
                var.AssignValue(IShaderVariable::TextureVarTuple{ nullptr, std::nullopt });
            });*/
    }
    return ErrorCode::ok;
}

std::error_code PrimitiveMaterial::assignVariableFunc(const std::string& semantic, Engine::EffectVariable::VariableValueAssignFunc fn)
{
    if (m_effectMaterial == nullptr) return ErrorCode::nullEffectMaterial;
    m_effectMaterial->setVariableAssignFunc(semantic, fn);
    return ErrorCode::ok;
}

void PrimitiveMaterial::changeSemanticTexture(const Engine::EffectTextureMap::EffectSemanticTextureTuple& tuple)
{
    m_effectTextureMap.changeSemanticTexture(tuple);
}

void PrimitiveMaterial::bindSemanticTexture(const Engine::EffectTextureMap::EffectSemanticTextureTuple& tuple)
{
    m_effectTextureMap.bindSemanticTexture(tuple);
}

void PrimitiveMaterial::bindSemanticTextures(const Engine::EffectTextureMap::SegmentEffectTextures& texture_tuples)
{
    for (const auto& tuple : texture_tuples)
    {
        bindSemanticTexture(tuple);
    }
}

void PrimitiveMaterial::changeEffect(const std::shared_ptr<Engine::EffectMaterial>& effectMaterial)
{
    m_effectMaterial = effectMaterial;
}

void PrimitiveMaterial::changeTextureMap(const Engine::EffectTextureMap& effectTextureMap)
{
    m_effectTextureMap = effectTextureMap;
}

void PrimitiveMaterial::selectVisualTechnique(const std::string& techniqueName)
{
    if (m_effectMaterial) m_effectMaterial->selectVisualTechnique(techniqueName);
}
