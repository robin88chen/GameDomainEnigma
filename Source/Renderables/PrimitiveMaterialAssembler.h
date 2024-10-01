/*********************************************************************
 * \file   PrimitiveMaterialAssembler.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   September 2024
 *********************************************************************/
#ifndef PRIMITIVE_MATERIAL_ASSEMBLER_H
#define PRIMITIVE_MATERIAL_ASSEMBLER_H

#include "GameEngine/GenericDto.h"
#include "GameEngine/EffectMaterialId.h"
#include "GameEngine/EffectMaterial.h"
#include "GameEngine/EffectTextureMapAssembler.h"

namespace Enigma::Renderables
{
    class PrimitiveMaterial;

    class PrimitiveMaterialAssembler
    {
    public:
        PrimitiveMaterialAssembler() = default;
        PrimitiveMaterialAssembler(const std::shared_ptr<Engine::EffectMaterial>& effect_material, const Engine::EffectTextureMap& effect_texture_map);
        PrimitiveMaterialAssembler(const Engine::EffectMaterialId& effect_material_id, const Engine::EffectTextureMap& effect_texture_map);
        virtual ~PrimitiveMaterialAssembler() = default;
        PrimitiveMaterialAssembler(const PrimitiveMaterialAssembler& other) = default;
        PrimitiveMaterialAssembler& operator=(const PrimitiveMaterialAssembler& other) = default;
        PrimitiveMaterialAssembler(PrimitiveMaterialAssembler&& other) noexcept = default;
        PrimitiveMaterialAssembler& operator=(PrimitiveMaterialAssembler&& other) noexcept = default;

        void effectMaterialId(const Engine::EffectMaterialId& id) { m_effectMaterialId = id; }
        void effectTextureMap(const Engine::EffectTextureMap& texture_map);
        void effectTextureMap(const std::shared_ptr<Engine::EffectTextureMapAssembler>& texture_map) { m_effectTextureMap = texture_map; }

        Engine::GenericDto assemble() const;
        static std::shared_ptr<PrimitiveMaterialAssembler> assembledAssemblerOf(const std::shared_ptr<PrimitiveMaterial>& material);

    protected:
        Engine::EffectMaterialId m_effectMaterialId;
        std::shared_ptr<Engine::EffectTextureMapAssembler> m_effectTextureMap;
    };

    class PrimitiveMaterialDisassembler
    {
    public:
        PrimitiveMaterialDisassembler() = default;
        PrimitiveMaterialDisassembler(const Engine::GenericDto& dto);
        virtual ~PrimitiveMaterialDisassembler() = default;
        PrimitiveMaterialDisassembler(const PrimitiveMaterialDisassembler& other) = default;
        PrimitiveMaterialDisassembler& operator=(const PrimitiveMaterialDisassembler& other) = default;
        PrimitiveMaterialDisassembler(PrimitiveMaterialDisassembler&& other) noexcept = default;
        PrimitiveMaterialDisassembler& operator=(PrimitiveMaterialDisassembler&& other) noexcept = default;

        [[nodiscard]] const Engine::EffectMaterialId& effectMaterialId() const { return m_effectMaterialId; }
        [[nodiscard]] const Engine::EffectTextureMap& effectTextureMap() const { return m_effectTextureMap; }

        void disassemble(const Engine::GenericDto& dto);

    protected:
        Engine::EffectMaterialId m_effectMaterialId;
        Engine::EffectTextureMap m_effectTextureMap;
    };
}

#endif // PRIMITIVE_MATERIAL_ASSEMBLER_H
