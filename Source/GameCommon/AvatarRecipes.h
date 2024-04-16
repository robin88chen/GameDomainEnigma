/*********************************************************************
 * \file   AvatarRecipes.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   April 2023
 *********************************************************************/
#ifndef _AVATAR_RECIPES_H
#define _AVATAR_RECIPES_H

#include "Frameworks/EventSubscriber.h"
#include "GameEngine/EffectTextureMapDto.h"
#include "Renderables/MeshPrimitive.h"
#include "SceneGraph/Pawn.h"
#include "GameEngine/EffectMaterialId.h"

namespace Enigma::GameCommon
{
    class AvatarRecipe
    {
        DECLARE_EN_RTTI_OF_BASE;
    public:
        AvatarRecipe();
        AvatarRecipe(const Engine::GenericDto& o);
        AvatarRecipe(const AvatarRecipe&) = delete;
        AvatarRecipe(AvatarRecipe&&) = delete;
        virtual ~AvatarRecipe() = default;
        AvatarRecipe& operator=(const AvatarRecipe&) = delete;
        AvatarRecipe& operator=(AvatarRecipe&&) = delete;

        const Engine::FactoryDesc& factoryDesc() const { return m_factoryDesc; }
        Engine::FactoryDesc& factoryDesc() { return m_factoryDesc; }

        virtual void bake(const std::shared_ptr<Enigma::SceneGraph::Pawn>& pawn) = 0;
        virtual Engine::GenericDto serializeDto() const = 0;

        static std::shared_ptr<AvatarRecipe> createFromGenericDto(const Engine::GenericDto& dto);

    protected:
        Engine::FactoryDesc m_factoryDesc;
    };

    class ReplaceAvatarMaterial : public AvatarRecipe
    {
        DECLARE_EN_RTTI;
    public:
        ReplaceAvatarMaterial(const Engine::EffectMaterialId& old_material_id, const Engine::EffectMaterialId& new_material_id);
        ReplaceAvatarMaterial(const Engine::GenericDto& o);
        ReplaceAvatarMaterial(const ReplaceAvatarMaterial&) = delete;
        ReplaceAvatarMaterial(ReplaceAvatarMaterial&&) = delete;
        virtual ~ReplaceAvatarMaterial() override;
        ReplaceAvatarMaterial& operator=(const ReplaceAvatarMaterial&) = delete;
        ReplaceAvatarMaterial& operator=(ReplaceAvatarMaterial&&) = delete;

        void bake(const std::shared_ptr<Enigma::SceneGraph::Pawn>& pawn) override;
        Engine::GenericDto serializeDto() const override;

    private:
        void replaceMeshMaterial(const std::shared_ptr<Renderables::MeshPrimitive>& mesh);
        void replaceNewMeshMaterialInSegment(const std::shared_ptr<Renderables::MeshPrimitive>& mesh, unsigned segment_index, const std::shared_ptr<Engine::EffectMaterial>& new_material);

        void onEffectMaterialHydrated(const Frameworks::IEventPtr& e);
        void onHydrateMaterialFailed(const Frameworks::IEventPtr& e);

    private:
        std::weak_ptr<Primitives::Primitive> m_primitive;
        Engine::EffectMaterialId m_oldMaterialId;
        Engine::EffectMaterialId m_newMaterialId;

        Frameworks::EventSubscriberPtr m_onEffectMaterialHydrated;
        Frameworks::EventSubscriberPtr m_onHydrateEffectMaterialFailed;
        using ChangeSpecifyMaterial = std::function<void(const Engine::EffectMaterialId&)>;
        std::unordered_map<Engine::EffectMaterialId, ChangeSpecifyMaterial, Engine::EffectMaterialId::hash> m_changeSpecifyMaterialMap;
    };
    class ChangeAvatarTexture : public AvatarRecipe
    {
        DECLARE_EN_RTTI;
    public:
        ChangeAvatarTexture(const Primitives::PrimitiveId& mesh_id, const Engine::TextureMappingDto& texture_dto);
        ChangeAvatarTexture(const Engine::GenericDto& o);
        ChangeAvatarTexture(const ChangeAvatarTexture&) = delete;
        ChangeAvatarTexture(ChangeAvatarTexture&&) = delete;
        virtual ~ChangeAvatarTexture() override;
        ChangeAvatarTexture& operator=(const ChangeAvatarTexture&) = delete;
        ChangeAvatarTexture& operator=(ChangeAvatarTexture&&) = delete;

        void bake(const std::shared_ptr<Enigma::SceneGraph::Pawn>& pawn) override;
        Engine::GenericDto serializeDto() const override;

    private:
        void changeMeshTexture(const std::shared_ptr<Renderables::MeshPrimitive>& mesh);
        void onTextureHydrated(const Frameworks::IEventPtr& e);
        void onHydrateTextureFailed(const Frameworks::IEventPtr& e);

    private:
        Primitives::PrimitiveId m_meshId;
        Engine::TextureMappingDto m_textureDto;
        Frameworks::EventSubscriberPtr m_onTextureHydrated;
        Frameworks::EventSubscriberPtr m_onHydrateTextureFailed;
        std::weak_ptr<Renderables::MeshPrimitive> m_mesh;
    };
}

#endif // _AVATAR_RECIPES_H
