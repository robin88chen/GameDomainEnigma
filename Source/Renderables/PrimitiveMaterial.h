/*********************************************************************
 * \file   PrimitiveMaterial.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   September 2024
 *********************************************************************/
#ifndef PRIMITIVE_MATERIAL_H
#define PRIMITIVE_MATERIAL_H

#include "Frameworks/LazyStatus.h"
#include "Frameworks/EventSubscriber.h"
#include "GameEngine/EffectMaterial.h"
#include "GameEngine/EffectTextureMap.h"

namespace Enigma::Renderables
{
    class PrimitiveMaterialAssembler;
    class PrimitiveMaterialDisassembler;
    class PrimitiveMaterial : public std::enable_shared_from_this<PrimitiveMaterial>
    {
        DECLARE_EN_RTTI_OF_BASE;
    public:
        PrimitiveMaterial();
        PrimitiveMaterial(const std::shared_ptr<Engine::EffectMaterial>& effectMaterial, const Engine::EffectTextureMap& effectTextureMap);
        PrimitiveMaterial(const Engine::EffectMaterialId& effectMaterialId, const Engine::EffectTextureMap& effectTextureMap);
        virtual ~PrimitiveMaterial();
        PrimitiveMaterial(const PrimitiveMaterial& other) = delete;
        PrimitiveMaterial& operator=(const PrimitiveMaterial& other) = delete;
        PrimitiveMaterial(PrimitiveMaterial&& other) noexcept = delete;
        PrimitiveMaterial& operator=(PrimitiveMaterial&& other) noexcept = delete;

        std::shared_ptr<PrimitiveMaterialAssembler> assembler() const;
        void assemble(const std::shared_ptr<PrimitiveMaterialAssembler>& assembler) const;
        std::shared_ptr<PrimitiveMaterialDisassembler> disassembler() const;
        void disassemble(const std::shared_ptr<PrimitiveMaterialDisassembler>& disassembler);

        [[nodiscard]] const std::shared_ptr<Engine::EffectMaterial>& effectMaterial() const { return m_effectMaterial; }
        [[nodiscard]] const Engine::EffectTextureMap& effectTextureMap() const { return m_effectTextureMap; }
        [[noddiscard]] Frameworks::LazyStatus& lazyStatus() { return m_lazyStatus; }

        void registerEventHandlersWhenNotReady();

        std::error_code assignShaderTextures();
        std::error_code unassignShaderTextures();
        std::error_code assignVariableFunc(const std::string& semantic, Engine::EffectVariable::VariableValueAssignFunc fn);

        void changeSemanticTexture(const Engine::EffectTextureMap::EffectSemanticTextureTuple& tuple);
        void bindSemanticTexture(const Engine::EffectTextureMap::EffectSemanticTextureTuple& tuple);
        void bindSemanticTextures(const Engine::EffectTextureMap::SegmentEffectTextures& texture_tuples);

        void changeEffect(const std::shared_ptr<Engine::EffectMaterial>& effectMaterial);
        void changeTextureMap(const Engine::EffectTextureMap& effectTextureMap);

        void selectVisualTechnique(const std::string& techniqueName);

    protected:
        void changeWhetherReady();
        bool shouldWaitHydration();

        void registerHydrationEventHandlers();
        void unregisterHydrationEventHandlers();

        void onEffectHydrated(const Frameworks::IEventPtr& e);
        void onEffectHydrationFailed(const Frameworks::IEventPtr& e);
        void onTextureHydrated(const Frameworks::IEventPtr& e);
        void onTextureHydrationFailed(const Frameworks::IEventPtr& e);

    protected:
        Frameworks::LazyStatus m_lazyStatus;
        std::shared_ptr<Engine::EffectMaterial> m_effectMaterial;
        Engine::EffectTextureMap m_effectTextureMap;

        Frameworks::EventSubscriberPtr m_onEffectHydrated;
        Frameworks::EventSubscriberPtr m_onEffectHydrationFailed;
        Frameworks::EventSubscriberPtr m_onTextureHydrated;
        Frameworks::EventSubscriberPtr m_onTextureHydrationFailed;
    };
}

#endif // PRIMITIVE_MATERIAL_H
