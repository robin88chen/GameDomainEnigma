/*********************************************************************
 * \file   AvatarRecipes.h
 * \brief
 *
 * \author Lancelot 'Robin' Chen
 * \date   April 2023
 *********************************************************************/
#ifndef _AVATAR_RECIPES_H
#define _AVATAR_RECIPES_H

#include "SceneGraph/Pawn.h"
#include "GameEngine/EffectMaterialDto.h"
#include "GameEngine/EffectTextureMapDto.h"
#include "Renderer/MeshPrimitive.h"
#include "Frameworks/EventSubscriber.h"

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

        const Engine::FactoryDesc& TheFactoryDesc() const { return m_factoryDesc; }
        Engine::FactoryDesc& TheFactoryDesc() { return m_factoryDesc; }

        virtual void Bake(const std::shared_ptr<Enigma::SceneGraph::Pawn>& pawn) = 0;
        virtual Engine::GenericDto SerializeDto() const = 0;

        static std::shared_ptr<AvatarRecipe> CreateFromGenericDto(const Engine::GenericDto& dto);

    protected:
        Engine::FactoryDesc m_factoryDesc;
    };

    class ReplaceAvatarMaterial : public AvatarRecipe
    {
        DECLARE_EN_RTTI;
    public:
        ReplaceAvatarMaterial(const std::string& old_material_name, const Engine::EffectMaterialDto& new_material_dto);
        ReplaceAvatarMaterial(const Engine::GenericDto& o);
        ReplaceAvatarMaterial(const ReplaceAvatarMaterial&) = delete;
        ReplaceAvatarMaterial(ReplaceAvatarMaterial&&) = delete;
        virtual ~ReplaceAvatarMaterial() override;
        ReplaceAvatarMaterial& operator=(const ReplaceAvatarMaterial&) = delete;
        ReplaceAvatarMaterial& operator=(ReplaceAvatarMaterial&&) = delete;

        void Bake(const std::shared_ptr<Enigma::SceneGraph::Pawn>& pawn) override;
        Engine::GenericDto SerializeDto() const override;

    private:
        void ReplaceMeshMaterial(const Renderer::MeshPrimitivePtr& mesh);

        void OnEffectMaterialCompiled(const Frameworks::IEventPtr& e);
        void OnCompileEffectMaterialFailed(const Frameworks::IEventPtr& e);

    private:
        std::weak_ptr<Engine::Primitive> m_primitive;
        std::string m_oldMaterialName;
        Engine::EffectMaterialDto m_newMaterialDto;

        Frameworks::EventSubscriberPtr m_onEffectMaterialCompiled;
        Frameworks::EventSubscriberPtr m_onCompileEffectMaterialFailed;
        using ChangeSpecifyMaterial = std::function<void(const std::shared_ptr<Engine::EffectMaterial>&)>;
        std::unordered_map<Frameworks::Ruid, ChangeSpecifyMaterial, Frameworks::Ruid::HashFunc> m_changeSpecifyMaterialMap;
    };
    class ChangeAvatarTexture : public AvatarRecipe
    {
        DECLARE_EN_RTTI;
    public:
        ChangeAvatarTexture(const std::string& mesh_name, const Engine::TextureMappingDto& texture_dto);
        ChangeAvatarTexture(const Engine::GenericDto& o);
        ChangeAvatarTexture(const ChangeAvatarTexture&) = delete;
        ChangeAvatarTexture(ChangeAvatarTexture&&) = delete;
        virtual ~ChangeAvatarTexture() override;
        ChangeAvatarTexture& operator=(const ChangeAvatarTexture&) = delete;
        ChangeAvatarTexture& operator=(ChangeAvatarTexture&&) = delete;

        void Bake(const std::shared_ptr<Enigma::SceneGraph::Pawn>& pawn) override;
        Engine::GenericDto SerializeDto() const override;

    private:
        void ChangeMeshTexture(const Renderer::MeshPrimitivePtr& mesh);
        void OnTextureLoaded(const Frameworks::IEventPtr& e);
        void OnLoadTextureFailed(const Frameworks::IEventPtr& e);

    private:
        std::string m_meshName;
        Engine::TextureMappingDto m_textureDto;
        Frameworks::EventSubscriberPtr m_onTextureLoaded;
        Frameworks::EventSubscriberPtr m_onLoadTextureFailed;
        std::weak_ptr<Renderer::MeshPrimitive> m_mesh;
        Frameworks::Ruid m_requsetRuid;
    };
}

#endif // _AVATAR_RECIPES_H
