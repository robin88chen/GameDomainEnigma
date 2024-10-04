/*********************************************************************
 * \file   AvatarRecipeAssemblers.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   September 2024
 *********************************************************************/
#ifndef AVATAR_RECIPE_ASSEMBLERS_H
#define AVATAR_RECIPE_ASSEMBLERS_H

#include "GameEngine/GenericDto.h"
#include "GameEngine/EffectMaterialId.h"
#include "Primitives/PrimitiveId.h"
#include "GameEngine/EffectSemanticTexture.h"

namespace Enigma::GameCommon
{
    class AvatarRecipeAssembler
    {
    public:
        AvatarRecipeAssembler();
        AvatarRecipeAssembler(const AvatarRecipeAssembler&) = default;
        AvatarRecipeAssembler(AvatarRecipeAssembler&&) = default;
        AvatarRecipeAssembler& operator=(const AvatarRecipeAssembler&) = default;
        AvatarRecipeAssembler& operator=(AvatarRecipeAssembler&&) = default;
        virtual ~AvatarRecipeAssembler() = default;

        void factory(const Engine::FactoryDesc& factoryDesc) { m_factoryDesc = factoryDesc; }

        virtual Engine::GenericDto assemble() const;

    protected:
        Engine::FactoryDesc m_factoryDesc;
    };
    class ReplaceAvatarMaterialAssembler : public AvatarRecipeAssembler
    {
    public:
        ReplaceAvatarMaterialAssembler();
        ReplaceAvatarMaterialAssembler(const ReplaceAvatarMaterialAssembler&) = default;
        ReplaceAvatarMaterialAssembler(ReplaceAvatarMaterialAssembler&&) = default;
        ReplaceAvatarMaterialAssembler& operator=(const ReplaceAvatarMaterialAssembler&) = default;
        ReplaceAvatarMaterialAssembler& operator=(ReplaceAvatarMaterialAssembler&&) = default;
        virtual ~ReplaceAvatarMaterialAssembler() override = default;

        void oldMaterialId(const Engine::EffectMaterialId& oldMaterialId) { m_oldMaterialId = oldMaterialId; }
        void newMaterialId(const Engine::EffectMaterialId& newMaterialId) { m_newMaterialId = newMaterialId; }

        Engine::GenericDto assemble() const override;

    protected:
        Engine::EffectMaterialId m_oldMaterialId;
        Engine::EffectMaterialId m_newMaterialId;
    };
    class ChangeAvatarTextureAssembler : public AvatarRecipeAssembler
    {
    public:
        ChangeAvatarTextureAssembler();
        ChangeAvatarTextureAssembler(const ChangeAvatarTextureAssembler&) = default;
        ChangeAvatarTextureAssembler(ChangeAvatarTextureAssembler&&) = default;
        ChangeAvatarTextureAssembler& operator=(const ChangeAvatarTextureAssembler&) = default;
        ChangeAvatarTextureAssembler& operator=(ChangeAvatarTextureAssembler&&) = default;
        virtual ~ChangeAvatarTextureAssembler() override = default;

        void meshId(const Primitives::PrimitiveId& meshId) { m_meshId = meshId; }
        void semanticTexture(const Engine::EffectSemanticTexture& semantic_texture) { m_semanticTexture = semantic_texture; }

        Engine::GenericDto assemble() const override;

    protected:
        Primitives::PrimitiveId m_meshId;
        Engine::EffectSemanticTexture m_semanticTexture;
    };

    class AvatarRecipeDisassembler
    {
    public:
        AvatarRecipeDisassembler();
        AvatarRecipeDisassembler(const AvatarRecipeDisassembler&) = default;
        AvatarRecipeDisassembler(AvatarRecipeDisassembler&&) = default;
        AvatarRecipeDisassembler& operator=(const AvatarRecipeDisassembler&) = default;
        AvatarRecipeDisassembler& operator=(AvatarRecipeDisassembler&&) = default;
        virtual ~AvatarRecipeDisassembler() = default;

        [[nodiscard]] const Engine::FactoryDesc& factory() const { return m_factoryDesc; }

        virtual void disassemble(const Engine::GenericDto& dto);

    protected:
        Engine::FactoryDesc m_factoryDesc;
    };
    class ReplaceAvatarMaterialDisassembler : public AvatarRecipeDisassembler
    {
    public:
        ReplaceAvatarMaterialDisassembler();
        ReplaceAvatarMaterialDisassembler(const ReplaceAvatarMaterialDisassembler&) = default;
        ReplaceAvatarMaterialDisassembler(ReplaceAvatarMaterialDisassembler&&) = default;
        ReplaceAvatarMaterialDisassembler& operator=(const ReplaceAvatarMaterialDisassembler&) = default;
        ReplaceAvatarMaterialDisassembler& operator=(ReplaceAvatarMaterialDisassembler&&) = default;
        virtual ~ReplaceAvatarMaterialDisassembler() override = default;

        [[nodiscard]] const Engine::EffectMaterialId& oldMaterialId() const { return m_oldMaterialId; }
        [[nodiscard]] const Engine::EffectMaterialId& newMaterialId() const { return m_newMaterialId; }

        void disassemble(const Engine::GenericDto& dto) override;

    protected:
        Engine::EffectMaterialId m_oldMaterialId;
        Engine::EffectMaterialId m_newMaterialId;
    };
    class ChangeAvatarTextureDisassembler : public AvatarRecipeDisassembler
    {
    public:
        ChangeAvatarTextureDisassembler();
        ChangeAvatarTextureDisassembler(const ChangeAvatarTextureDisassembler&) = default;
        ChangeAvatarTextureDisassembler(ChangeAvatarTextureDisassembler&&) = default;
        ChangeAvatarTextureDisassembler& operator=(const ChangeAvatarTextureDisassembler&) = default;
        ChangeAvatarTextureDisassembler& operator=(ChangeAvatarTextureDisassembler&&) = default;
        virtual ~ChangeAvatarTextureDisassembler() override = default;

        [[nodiscard]] const Primitives::PrimitiveId& meshId() const { return m_meshId; }
        [[nodiscard]] const Engine::EffectSemanticTexture& semanticTexture() const { return m_semanticTexture; }

        void disassemble(const Engine::GenericDto& dto) override;

    protected:
        Primitives::PrimitiveId m_meshId;
        Engine::EffectSemanticTexture m_semanticTexture;
    };
}

#endif // AVATAR_RECIPE_ASSEMBLERS_H
