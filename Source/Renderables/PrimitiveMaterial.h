/*********************************************************************
 * \file   PrimitiveMaterial.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   September 2024
 *********************************************************************/
#ifndef PRIMITIVE_MATERIAL_H
#define PRIMITIVE_MATERIAL_H

#include "GameEngine/EffectMaterial.h"
#include "GameEngine/EffectTextureMap.h"

namespace Enigma::Renderables
{
    class PrimitiveMaterialAssembler;
    class PrimitiveMaterialDisassembler;
    class PrimitiveMaterial
    {
        DECLARE_EN_RTTI_OF_BASE;
    public:
        PrimitiveMaterial();
        PrimitiveMaterial(const std::shared_ptr<Engine::EffectMaterial>& effectMaterial, const Engine::EffectTextureMap& effectTextureMap);
        PrimitiveMaterial(const Engine::EffectMaterialId& effectMaterialId, const Engine::EffectTextureMap& effectTextureMap);
        virtual ~PrimitiveMaterial() = default;
        PrimitiveMaterial(const PrimitiveMaterial& other) = default;
        PrimitiveMaterial& operator=(const PrimitiveMaterial& other) = default;
        PrimitiveMaterial(PrimitiveMaterial&& other) noexcept = default;
        PrimitiveMaterial& operator=(PrimitiveMaterial&& other) noexcept = default;

        std::shared_ptr<PrimitiveMaterialAssembler> assembler() const;
        void assemble(const std::shared_ptr<PrimitiveMaterialAssembler>& assembler) const;
        std::shared_ptr<PrimitiveMaterialDisassembler> disassembler() const;
        void disassemble(const std::shared_ptr<PrimitiveMaterialDisassembler>& disassembler);

        [[nodiscard]] const std::shared_ptr<Engine::EffectMaterial>& effectMaterial() const { return m_effectMaterial; }
        [[nodiscard]] const Engine::EffectTextureMap& effectTextureMap() const { return m_effectTextureMap; }

        std::error_code assignShaderTextures();
        std::error_code unassignShaderTextures();

        void changeSemanticTexture(const Engine::EffectTextureMap::EffectSemanticTextureTuple& tuple);
        void bindSemanticTexture(const Engine::EffectTextureMap::EffectSemanticTextureTuple& tuple);
        void bindSemanticTextures(const Engine::EffectTextureMap::SegmentEffectTextures& texture_tuples);

        void changeEffect(const std::shared_ptr<Engine::EffectMaterial>& effectMaterial);
        void changeTextureMap(const Engine::EffectTextureMap& effectTextureMap);

        void selectVisualTechnique(const std::string& techniqueName);

    protected:
        std::shared_ptr<Engine::EffectMaterial> m_effectMaterial;
        Engine::EffectTextureMap m_effectTextureMap;
    };
}

#endif // PRIMITIVE_MATERIAL_H
