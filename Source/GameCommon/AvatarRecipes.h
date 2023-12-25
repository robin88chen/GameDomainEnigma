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
        void replaceMeshMaterial(const Renderer::MeshPrimitivePtr& mesh);
        void replaceNewMeshMaterialInSegment(const Renderer::MeshPrimitivePtr& mesh, unsigned segment_index, const std::shared_ptr<Engine::EffectMaterial>& new_material);

        void onEffectMaterialContented(const Frameworks::IEventPtr& e);
        void onContentMaterialFailed(const Frameworks::IEventPtr& e);

    private:
        std::weak_ptr<Engine::Primitive> m_primitive;
        Engine::EffectMaterialId m_oldMaterialId;
        Engine::EffectMaterialId m_newMaterialId;

        Frameworks::EventSubscriberPtr m_onEffectMaterialContented;
        Frameworks::EventSubscriberPtr m_onContentEffectMaterialFailed;
        using ChangeSpecifyMaterial = std::function<void(const Engine::EffectMaterialId&)>;
        std::unordered_map<Engine::EffectMaterialId, ChangeSpecifyMaterial, Engine::EffectMaterialId::hash> m_changeSpecifyMaterialMap;
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

        void bake(const std::shared_ptr<Enigma::SceneGraph::Pawn>& pawn) override;
        Engine::GenericDto serializeDto() const override;

    private:
        void changeMeshTexture(const Renderer::MeshPrimitivePtr& mesh);
        void onTextureLoaded(const Frameworks::IEventPtr& e);
        void onLoadTextureFailed(const Frameworks::IEventPtr& e);

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
